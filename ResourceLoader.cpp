#include "ResourceLoader.hpp"

Uint16 ResourceLoader::load(const char* p_filepath) {
	SDL_Texture* texture = NULL;
	texture = IMG_LoadTexture(renderer, p_filepath);

	if (texture == NULL) {
		std::cout << "Failed to load texture: " << SDL_GetError() << std::endl;
		return NULL;
	}
	else {
		textures.push_back(texture);
		return currentID;
		currentID++;
	}
}
SDL_Texture* ResourceLoader::getTex(Uint16 p_ID) {
	return textures[p_ID];
	//for (int i = 0; i < IDs.size(); i++) {
	//	if (IDs[i] == p_ID) {
	//		return textures[i];
	//	}
	//}
}