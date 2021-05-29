#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include "entity.hpp"
#include "utils.h"
#include "World.hpp"
#include "Tile.hpp"
#include <iostream>
class World;

class RenderWindow
{
public:
	RenderWindow(const char* p_title, int p_w, int p_h);
	SDL_Texture* loadTexture(const char* p_filePath);
	void cleanUp();
	void clear();
	void renderEntity(Entity& p_entity);
	void drawWorld(World& p_world);
	void display();
	int getRefreshRate();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
};
