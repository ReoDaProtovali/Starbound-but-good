#include "ResourceLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION   // use of stb functions once and for all
#include <util/ext/stb_image.h>
#include <util/ext/glm/glm.hpp>
#include "GameConstants.hpp"

bool ResourceLoader::load(const char* p_filepath, TextureID p_ID) {
#ifdef LOADLOGGING_ENABLED
	std::cout << "Loading image resource at " << p_filepath << " with ID " << (unsigned int)p_ID << std::endl;
#endif
	if (textures.find(p_ID) != textures.end()) {
		std::cout << "Texture with ID " << (unsigned int)p_ID << " already exists." << std::endl;
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
		textures.insert(std::make_pair(p_ID, std::make_shared<Texture>(loadedTexture)));

		delete imageData;
	}
	return true;
}

std::shared_ptr<Texture> ResourceLoader::getTexture(TextureID p_ID, bool& p_success) {
	auto it = textures.find(p_ID);
	if (it != textures.end()) {
		p_success = true;
		return it->second;
	}
	p_success = false;
	return std::make_shared<Texture>(Texture());
}
std::shared_ptr<Texture> ResourceLoader::getTexture(TextureID p_ID) {
	auto it = textures.find(p_ID);
	if (it != textures.end()) {
		return it->second;
	}
	throw std::invalid_argument("Failed to get image: ID Not found.");
}
