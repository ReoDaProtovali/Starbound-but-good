#include "ResourceLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION   // use of stb functions once and for all
#include <util/ext/stb_image.h>
#include <util/ext/glm/glm.hpp>
#include "GameConstants.hpp"


ResourceLoader::ResourceLoader()
{
	LOAD_LOG("Creating Resource loader...");
	tileSheetPixmap.resize(96, 24);
}

ResourceLoader::~ResourceLoader()
{
	std::map<TextureID, Texture>::iterator it;
	for (it = textures.begin(); it != textures.end(); it++) {
		it->second.remove();
	}
}
bool ResourceLoader::loadTexID(const char* p_filepath, TextureID p_ID) {
	LOAD_LOG("Loading image resource at " << p_filepath << " with ID " << (uint32_t)p_ID);

	if (textures.find(p_ID) != textures.end()) {
		std::cout << "Texture with ID " << (uint32_t)p_ID << " already exists." << std::endl;
		return true;
	};
	unsigned char* imageData = nullptr;
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	imageData = stbi_load(p_filepath, &width, &height, &nrChannels, 0);

	if (imageData == nullptr) {
		std::cout << "Failed to load image" << std::endl;
		throw std::runtime_error("what");
		return false;
	}
	else {
		Texture loadedTexture = Texture(p_ID);
		loadedTexture.fromByteData(width, height, imageData);
		textures.insert(std::make_pair(p_ID, loadedTexture));

		delete imageData;
	}
	return true;
}

Texture* ResourceLoader::getTexture(TextureID p_ID, bool& p_success) {
	auto it = textures.find(p_ID);
	if (it != textures.end()) {
		p_success = true;
		return &it->second;
	}
	p_success = false;
	return nullptr;
}

Texture* ResourceLoader::getTileSheetTexture() {
	if (!tileSheetTexture.initialized) {
		tileSheetTexture.fromVec4Data(tileSheetPixmap.width, tileSheetPixmap.height, tileSheetPixmap.getData());
	}
	return &tileSheetTexture;
}

void ResourceLoader::loadAllTileSets() {
	namespace fs = std::filesystem;
	for (const auto& entry : fs::directory_iterator(fs::current_path() / "res\\tilesets")) {
		if (entry.is_directory()) {
			loadTileSet(entry.path());
		}
	}

}
void ResourceLoader::loadTileSet(std::filesystem::path p_tileSetPath)
{
	namespace fs = std::filesystem;
	LOAD_LOG("Loading " << p_tileSetPath.string() << "...");
	if (!fs::exists(p_tileSetPath)) {
		throw std::invalid_argument("Tileset path does not exist.");
		return;
	}
	bool jsonFound = false;
	for (const auto& entry : fs::directory_iterator(p_tileSetPath)) {
		if (entry.path().filename().string() == "tileset.json") jsonFound = true;
		if (jsonFound) {
			const std::string path = entry.path().string();
			using namespace nlohmann;
			json tileSetData = json::parse(utils::readFile(path.c_str()));
			std::string nspace = tileSetData["namespace"];
			fs::path imageDirectory = p_tileSetPath / tileSetData["imageDirectory"];
			fs::path tileDirectory = p_tileSetPath / tileSetData["tileDirectory"];
			loadDirTiles(nspace, tileDirectory, imageDirectory, p_tileSetPath);
			return;
		}
	}
	LOG("tileset.json not found!");
}

void ResourceLoader::loadDirTiles(std::string p_namespace, std::filesystem::path p_tileInfoPath, std::filesystem::path p_imagePath, std::filesystem::path p_parentPath) {
	namespace fs = std::filesystem;
	for (const auto& entry : fs::directory_iterator(p_tileInfoPath)) {
		const std::string filename = entry.path().filename().string();
		std::string filenameLower = filename;
		std::transform(filenameLower.begin(), filenameLower.end(), filenameLower.begin(), [](unsigned char c) {return std::tolower(c); });

		if (!filenameLower.ends_with(".tileproperties")) {
			WARNING_LOG("Rouge file in tileset " << p_namespace << ". Typo? Name: " << filename);
			continue;
		}

		using namespace nlohmann;

		// First, parse the json.
		json tileInfoJSON;
		try {
			tileInfoJSON = json::parse(utils::readFile(entry.path().string().c_str()));
		}
		catch (json::parse_error e) {
			ERROR_LOG("Tile data JSON failed to parse at " << filename << " in namespace " << p_namespace);
			return;
		}

		// Then, try to load it into our custom struct.
		TileInfo tileInfo;
		try {
			std::string tmpID = tileInfoJSON["id"];
			tileInfo = {
			   p_namespace + ":" + tmpID,
			   tileInfoJSON["name"],
			   tileInfoJSON["genericDescription"],
			   tileInfoJSON["categories"],
			   tileInfoJSON["rarity"],
			   tileInfoJSON["imageFile"],
			   999999,
			   tileInfoJSON["isAnimated"],
			   tileInfoJSON["emissive"],
			   tileInfoJSON["lightingColor"],
			   tileInfoJSON["maxHP"],
			   tileInfoJSON["variationCount"],
			   tileInfoJSON["lightAbsorption"]
			};
		}
		catch (json::type_error e) {
			ERROR_LOG("Tile data JSON is missing one or more properties at " << filename << " in namespace " << p_namespace << ". Exception: " << e.what());
			return;
		}
		tileInfoCache.push_back(tileInfo);
		tileInfoIndexDict[tileInfo.id] = tileInfoCache.size() - 1;

		// Now we try to bind the image's correct index into the tile data.
		fs::path tileImagePath = p_imagePath / tileInfo.imageFile;
		if (!fs::exists(tileImagePath)) {
			ERROR_LOG("Tile's image file was not found. Tried to find " << (p_imagePath / tileInfo.imageFile).string() << " and failed.");
			return;
		}

		unsigned char* imageData = nullptr;
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		imageData = stbi_load(tileImagePath.string().c_str(), &width, &height, &nrChannels, 0);

		if (imageData == nullptr) {
			ERROR_LOG("Failed to load image");
			return;
		}

		tileSheetPixmap.appendImage(imageData, width * height * nrChannels);
		delete imageData;

		static size_t imgIndex = 0;
		tileInfoCache.back().imageIndex = ++imgIndex;

		LOG(tileInfo.name << ": Internal ID - " << tileInfoCache.back().imageIndex);

		LOG("Loaded tile " << tileInfo.id << " and successfully stitched image!");
	}
	tileSheetPixmap.reverse();
}
std::optional<std::reference_wrapper<TileInfo>> ResourceLoader::getTileInfo(std::string p_key)
{
	auto it = tileInfoIndexDict.find(p_key);
	if (it == tileInfoIndexDict.end()) {
		return std::nullopt;
	}
	return tileInfoCache[it->second];
}
TileInfo& ResourceLoader::getTileInfo(size_t p_index)
{
	return tileInfoCache[p_index];
}
Texture* ResourceLoader::getTexture(TextureID p_ID) {
	auto it = textures.find(p_ID);
	if (it != textures.end()) {
		return &it->second;
	}
	throw std::invalid_argument("Failed to get image: ID Not found.");
}
