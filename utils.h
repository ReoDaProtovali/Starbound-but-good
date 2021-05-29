#pragma once
#include <SDL.h>
#include "Renderwindow.hpp"


namespace utils {
	inline float hireTimeInSeconds() {
		float t = (float)SDL_GetTicks();
		t *= 0.001f;
		return t;
	}

}

