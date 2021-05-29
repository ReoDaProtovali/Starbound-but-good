#pragma once
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
class ResourceLoader
{
public:
	ResourceLoader() : renderer(NULL) {};
	ResourceLoader(SDL_Renderer* p_renderer) : renderer(p_renderer) {};
	Uint16 load(const char* p_filepath);
	SDL_Texture* getTex(Uint16 p_ID);
private:
	Uint16 currentID = 1;
	SDL_Renderer* renderer;
	std::vector<SDL_Texture*> textures;
	std::vector<std::string> IDs;
};

