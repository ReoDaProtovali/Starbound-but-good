#include "ResourceLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION   // use of stb functions once and for all
#include "stb_image.h"

Uint16 ResourceLoader::load(const char* p_filepath) {
	unsigned char* texture = NULL;
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	texture = stbi_load(p_filepath, &width, &height, &nrChannels, 0);

	if (texture == NULL) {
		std::cout << "Failed to load texture" << std::endl;
		return NULL;
	}
	else {
		textures.push_back(texture);
		dimensions.push_back(Vector2i(width, height));
		return currentID++;
	}
}

unsigned char* ResourceLoader::getTex(Uint16 p_ID) {
	return textures[p_ID];
}
Vector2i ResourceLoader::getDimensions(Uint16 p_ID) {
	return dimensions[p_ID];
}