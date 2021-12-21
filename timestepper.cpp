#include "Timestepper.hpp"

Timestepper::Timestepper(float p_gameUpdateFPS)
	: gameUpdateFPS(p_gameUpdateFPS), accumulator(0.0f), currentTime(utils::hireTimeInSeconds()), alpha(0.0f), startTicks(0), frameTicks(0), frameTime(0.0f)
{}
void Timestepper::processFrameStart() {
	startTicks = SDL_GetTicks();
	float newTime = utils::hireTimeInSeconds();
	float frameTime = newTime - currentTime;
	currentTime = newTime;
	accumulator += frameTime;
}
bool Timestepper::accumulatorFull() {
	return accumulator >= 1 / gameUpdateFPS;
}

void Timestepper::calculateAlpha() {
	alpha = accumulator * gameUpdateFPS; // did some quick maf, same thing as accumulator / (1 / gameUpdateFPS)
}
void Timestepper::processFrameEnd(GameWindow& p_window) {
	frameTicks = SDL_GetTicks() - startTicks;
	if (frameTicks < (1000.0f / p_window.getRefreshRate())) {
		SDL_Delay((Uint32)(1000.0f / p_window.getRefreshRate()) - frameTicks);
	}
}