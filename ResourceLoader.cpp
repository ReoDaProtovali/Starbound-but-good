#include "ResourceLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION   // use of stb functions once and for all
#include "stb_image.h"
#include "glm/glm.hpp"

bool ResourceLoader::load(const char* p_filepath, TextureID p_ID) {
	if (textures.find(p_ID) != textures.end()) return true;
	unsigned char* imageData = nullptr;
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	imageData = stbi_load(p_filepath, &width, &height, &nrChannels, 0);

	if (imageData == nullptr) {
		std::cout << "Failed to load image" << std::endl;
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

Texture ResourceLoader::getTexture(TextureID p_ID, bool& success) {
	auto it = textures.find(p_ID);
	if (it != textures.end()) {
		success = true;
		return it->second;
	}
	success = false;
	return Texture();
}
Texture ResourceLoader::getTexture(TextureID p_ID) {
	auto it = textures.find(p_ID);
	if (it != textures.end()) {
		return it->second;
	}
	throw std::invalid_argument("Failed to get image: ID Not found.");
}
