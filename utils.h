#pragma once
#include <SDL.h>
constexpr auto SIXTY_TIMES_PER_SECOND = 0.01666666666f;

namespace utils {
	inline float hireTimeInSeconds() {
		float t = (float)SDL_GetTicks();
		t *= 0.001f;
		return t;
	}

}
