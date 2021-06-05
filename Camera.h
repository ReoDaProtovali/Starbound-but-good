#pragma once
#include <SDL.h>
#include "MathUtils.hpp"
#include "InputHandler.h"
struct Camera
{
	Camera() : frame(SDL_Rect()), scale(NULL) {};
	Camera(SDL_Rect p_frame, float p_scale) : frame(p_frame), scale(p_scale) {};
	float scale;
	SDL_Rect frame;
	void testUpdate(InputHandler& p_inph, SDL_Window* p_window);
};

