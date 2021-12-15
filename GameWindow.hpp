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
	void cleanUp();
	void clear();
	void renderEntity(Entity& p_entity); // gonna be moved
	void drawChunk(WorldChunk& p_chunk); // outdated
	void display();
	void updateCamera();
	int getRefreshRate();
	int width;
	int height;
	ResourceLoader res;
	Camera cam;
	InputHandler inpHandler;
	SDL_GLContext glContext;
	SDL_Window* window;
	SDL_Renderer* renderer;
};
