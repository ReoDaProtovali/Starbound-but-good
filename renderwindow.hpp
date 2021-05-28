#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include "entity.hpp"
#include "utils.h"

#include <iostream>
class RenderWindow
{
public:
	RenderWindow(const char* p_title, int p_w, int p_h);
	SDL_Texture* loadTexture(const char* p_filePath);
	void cleanUp();
	void clear();
	void render(Entity& p_entity);
	void display();
	int getRefreshRate();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
};
