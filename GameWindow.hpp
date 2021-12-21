#pragma once
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>

#include "entity.hpp"
#include "utils.hpp"
#include "World.hpp"
#include "Chunk.hpp"
#include "Tile.hpp"
#include "ResourceLoader.hpp"
#include "InputHandler.hpp"
#include <iostream>
#include <math.h>


class World;
struct WorldChunk;
class GameWindow
{
public:
	GameWindow(const char* p_title, int p_w, int p_h);
	GameWindow(const char* p_title);

	void initGL();
	void cleanUp();
	int getRefreshRate();
	int width;
	int height;
	InputHandler inpHandler;
	SDL_Window* window;
};
