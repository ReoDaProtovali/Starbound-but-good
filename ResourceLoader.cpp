#include "ResourceLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION   // use of stb functions once and for all
#include "stb_image.h"

bool ResourceLoader::load(const char* p_filepath, TextureID p_ID) {
	unsigned char* texture = NULL;
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	texture = stbi_load(p_filepath, &width, &height, &nrChannels, 0);

	if (texture == NULL) {
		std::cout << "Failed to load texture" << std::endl;
		return NULL;
	}
	else {
		Image loadedImage = Image(texture, Vector2i(width, height), p_ID);
		images.push_back(loadedImage);
	}
	return 1;
}

Image ResourceLoader::getImage(TextureID p_ID) {
	for (int i = 0; i < images.size(); i++) {
		if (images[i].ID == p_ID) {
			return images[i];
		}
	}
	throw std::invalid_argument("Failed to get image: ID Not found.");
}
