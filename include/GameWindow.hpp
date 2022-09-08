#pragma once
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>

#include "util/utils.hpp"
#include "Chunk.hpp"
#include "ResourceLoader.hpp"
#include "InputHandler.hpp"
#include <iostream>
#include <math.h>

class World;
struct WorldChunk;
class GameWindow
{
public:
	/** Constructor
	* @param p_title - The text displayed on the top window bar
	* @param p_w, p_h - The width and height of the window in pixels.
	*/
	GameWindow(const char* p_title, int p_w, int p_h);
	/** Constructor, 
	* Defaulted to 1280x720.
	* @param p_title - The text displayed on the top window bar
	*/
	GameWindow(const char* p_title);

	/// Does all the work to initialize OpenGL rendering for SDL and GLEW
	void initGL();
	/// Deletes the window when we are done using it.
	void cleanUp();
	/**
	* @returns The refresh rate of the screen the window is on.
	*/
	int getRefreshRate();
	/// Tells all subsequent draw calls to render directly to the display window.
	void bindAsRenderTarget();
	// Swaps the doublebuffer, and shows the new frame.
	void displayNewFrame();
	int width;
	int height;
	unsigned int screenWidth;
	unsigned int screenHeight;
	/// SDL managed OpenGL context. Not used for much outside of the constructor.
	SDL_GLContext glContext;
	/// The input handler for the game window. 
	InputHandler inpHandler;
	/// A pointer to the SDL window, used internally in most cases.
	SDL_Window* window;
};
