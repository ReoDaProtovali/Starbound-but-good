#include "ResourceManager.hpp"
#define STB_IMAGE_IMPLEMENTATION   // use of stb functions once and for all
#include <util/ext/stb_image.h>
#include <util/ext/glm/glm.hpp>
#include "GameConstants.hpp"


ResourceManager::ResourceManager()
{
	LOAD_LOG("Creating Resource manager...");
	tileSheetPixmap.resize(96, 24);
}

ResourceManager::~ResourceManager()
{
	for (auto& [str, tex] : textures) {
		tex.remove();
	}
	m_generatorShaders.clear();
	tileInfoIndexDict.clear();
}

Texture ResourceManager::getTexture(const std::string& p_ID, bool& p_success) {
	if (!textures.contains(p_ID)) {
		p_success = false;
		return Texture();
	};
	return textures[p_ID];
}

Texture ResourceManager::getTileSheetTexture() {
	if (!tileSheetTexture.initialized) {
		tileSheetTexture.setFiltering(GL_NEAREST, GL_NEAREST);
		tileSheetTexture.fromVec4Data(tileSheetPixmap.width, tileSheetPixmap.height, tileSheetPixmap.getData());
		//tileSheetTexture.genMipMapsFloat(0, tileSheetPixmap.width, tileSheetPixmap.height, (float*)tileSheetPixmap.getData());
	}
	return tileSheetTexture;
}

void ResourceManager::loadAllTileSets() {
	namespace fs = std::filesystem;
	for (const auto& entry : fs::directory_iterator(fs::current_path() / TILESET_PATH)) {
		if (entry.is_directory()) {
			loadTileSet(entry.path());
		}
	}

}
void ResourceManager::loadTileSet(const std::filesystem::path& p_tileSetPath)
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

void ResourceManager::loadDirTiles(const std::string& p_namespace, const std::filesystem::path& p_tileInfoPath, const std::filesystem::path& p_imagePath, const std::filesystem::path& p_parentPath) {
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
			   tileInfoJSON["transparent"],
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
		// assume rgba
		glm::vec4 lightingCol = glm::vec4(tileInfo.lightingColor[0], tileInfo.lightingColor[1], tileInfo.lightingColor[2], 1.f - tileInfo.lightAbsorption);
		tileSheetPixmap.setPixel(
			tileSheetPixmap.width - 1,
			tileSheetPixmap.height - 1,
			lightingCol);


		static size_t imgIndex = 0;
		tileInfoCache.back().spriteIndex = (uint32_t)++imgIndex;

		LOG(tileInfo.name << ": Internal ID - " << tileInfoCache.back().spriteIndex);

		LOG("Loaded tile " << tileInfo.id << " and successfully stitched image!");
	}
	tileSheetPixmap.reverse();

	uint32_t paddedSize = 96;
	while (paddedSize < tileSheetPixmap.height) {
		paddedSize *= 2;
	}

	if (tileSheetPixmap.height % paddedSize != 0) {
		uint32_t diff = paddedSize - tileSheetPixmap.height % paddedSize;
		tileSheetPixmap.prependEmpty(diff);
	}

	tileSheetPixmap.toPNG("testpixmap.png");
}
std::optional<std::reference_wrapper<TileInfo>> ResourceManager::getTileInfo(const std::string& p_key)
{
	auto it = tileInfoIndexDict.find(p_key);
	if (it == tileInfoIndexDict.end()) {
		return std::nullopt;
	}
	return tileInfoCache[it->second];
}
std::optional<uint32_t> ResourceManager::getTileNumericalID(const std::string& p_key)
{
	auto it = tileInfoIndexDict.find(p_key);
	if (it == tileInfoIndexDict.end()) {
		return std::nullopt;
	}
	return tileInfoCache[it->second].spriteIndex;
}
TileInfo& ResourceManager::getTileInfo(size_t p_index)
{
	return tileInfoCache[p_index];
}
bool ResourceManager::loadTexID(const char* p_filepath, std::string_view p_assignedID)
{
	LOAD_LOG("Loading image resource at " << p_filepath << " with ID " << p_assignedID);
	std::string idStr(p_assignedID);

	if (textures.contains(idStr)) {
		std::cout << "Texture with ID " << idStr << " already exists." << std::endl;
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
		Texture loadedTexture = Texture(idStr);
		loadedTexture.fromByteData(width, height, imageData);
		textures.insert(std::make_pair(idStr, loadedTexture));

		delete imageData;
	}
	return true;
}
Texture ResourceManager::getTexture(const std::string& p_ID) {
	if (!textures.contains(p_ID)) throw std::invalid_argument("Failed to get image: ID Not found.");
	return textures[p_ID];
}

void ResourceManager::loadGeneratorShaders() {
	LOG("LOADING GENERATOR SHADERSS");
	namespace fs = std::filesystem;
	fs::path generatorsJSONPath = GENERATORS_JSON_PATH;
	if (!fs::exists(generatorsJSONPath)) {
		ERROR_LOG("Unable to find generators.json! Shaders not loaded.");
		return;
	}
	using namespace nlohmann;
	// First, parse the json.
	json generatorsJSON;
	try {
		generatorsJSON = json::parse(utils::readFile(generatorsJSONPath.string().c_str()));
	}
	catch (json::parse_error e) {
		ERROR_LOG("Failed to parse noise generator JSON.\n" << e.what());
		return;
	}


	try {
		std::vector<std::map<std::string, std::string>> generators = generatorsJSON["generators"];
		for (auto e : generators) {
			fs::path vertexShaderPath = generatorsJSONPath.parent_path() / "PassThroughVS.glsl";
			fs::path fragmentShaderPath = generatorsJSONPath.parent_path() / e["file"];
			m_generatorShaders[e["name"]] = Shader(vertexShaderPath.string().c_str(), fragmentShaderPath.string().c_str());
			m_generatorShaders[e["name"]].addIntUniform("Seed", 0);
			m_generatorShaders[e["name"]].addVec2Uniform("WorldPos", glm::vec2(0));
			m_generatorShaders[e["name"]].addFloatUniform("WorldSize", 1.f);

		}
	}
	catch (json::type_error e) {
		ERROR_LOG("Failed to read noise generator JSON property." << ". Exception: " << e.what());
		return;
	}

}

Shader& ResourceManager::getGeneratorShader(const std::string& p_name) {
	if (!m_generatorShaders.contains(p_name)) {
		ERROR_LOG("Failed to get requested shader: " + p_name);
		throw std::exception("Try again nerd");
	}
	return m_generatorShaders[p_name];
}

std::vector<std::string> ResourceManager::getAllGeneratorShaders()
{
	std::vector<std::string> outVec;
	for (auto& it : m_generatorShaders) {
		outVec.emplace_back(it.first);
	}
	return outVec;
}
