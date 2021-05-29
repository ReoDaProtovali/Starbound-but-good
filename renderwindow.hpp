#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include "entity.hpp"
#include "utils.h"
#include "World.hpp"
#include "Chunk.hpp"
#include "Tile.hpp"
#include "ResourceLoader.hpp"
#include <iostream>

class World;
class WorldChunk;
class RenderWindow
{
public:
	RenderWindow(const char* p_title, int p_w, int p_h);
	void cleanUp();
	void clear();
	void renderEntity(Entity& p_entity);
	void drawChunk(WorldChunk& p_chunk);
	void display();
	int getRefreshRate();
	ResourceLoader res;
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
};
