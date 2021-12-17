#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>

#include "entity.hpp"
#include "utils.h"
#include "World.hpp"
#include "Chunk.hpp"
#include "Tile.hpp"
#include "ResourceLoader.hpp"
#include "Camera.h"
#include "InputHandler.h"
#include <iostream>
#include <math.h>


class World;
class WorldChunk;
class GameWindow
{
public:
	GameWindow(const char* p_title, int p_w, int p_h);
	void initGL();
	void cleanUp();
	int getRefreshRate();
	int width;
	int height;
	Camera cam;
	InputHandler inpHandler;
	SDL_GLContext glContext;
	SDL_Window* window;
	SDL_Renderer* renderer;
};
