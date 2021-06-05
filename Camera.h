#pragma once
#include <SDL.h>
#include "MathUtils.hpp"
#include "InputHandler.h"
#include <math.h>
struct Camera
{
	Camera() : frame(SDL_Rect()), xyscale(Vector2f()), globalScale(16), aspectRatio(1) {};
	Camera(Vector2f pos, Vector2i p_dimensions) : xyscale(Vector2f()), globalScale(16), aspectRatio(1) {
		aspectRatio = (float)p_dimensions.x / (float)p_dimensions.y; // not sure why I even added an aspect ratio, things will probably just be renderered with it set to 1
		xyscale.x = aspectRatio * globalScale; // i guess if you ever want things to be stretched you can if you want to
		xyscale.y = (1 / aspectRatio) * globalScale;
		frame.x = std::floorf(pos.x - (float)p_dimensions.x / 2);
		frame.y = std::floorf(pos.y - (float)p_dimensions.y / 2);
		frame.w = p_dimensions.x;
		frame.h = p_dimensions.y; // this code im writing is melting my poor brain please don't try to understand it
	};
	Vector2f pos = Vector2f(0, 0);
	SDL_Rect frame = SDL_Rect();
	Vector2f xyscale;
	float aspectRatio;
	float globalScale;
	void testUpdate(InputHandler& p_inph, SDL_Window* p_window);
	void recalculateView(SDL_Window* p_window);
};

