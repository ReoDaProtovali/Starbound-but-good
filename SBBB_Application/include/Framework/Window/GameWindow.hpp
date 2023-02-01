#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>

#include "Framework/Graphics/DrawSurface.hpp"
#include "util/utils.hpp"
#include "Framework/Input/InputHandler.hpp"
#include <iostream>
#include <math.h>

class DrawSurface;
class GameWindow : public DrawSurface
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

	/// Attaches GL to SDL window
	void initGL();
	/// Deletes the window when we are done using it.
	void cleanUp();
	/**
	* @returns The refresh rate of the screen the window is on.
	*/
	int getRefreshRate();

	// Used to disable the framerate limit.
	void setVSync(bool p_enabled);
	// Swaps the doublebuffer, and shows the new frame.
	void displayNewFrame();


	int windowWidth;
	int windowHeight;
	uint32_t screenWidth;
	uint32_t screenHeight;

	static bool OpenGLInitialized;

	/// SDL managed OpenGL context. Not used for much outside of the constructor.
	SDL_GLContext m_glContext;

	/// A pointer to the SDL window, used internally in most cases.
	SDL_Window* m_window;
private:

};
#endif