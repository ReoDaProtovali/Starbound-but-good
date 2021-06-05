#include "Timestepper.h"

Timestepper::Timestepper(float p_timestep)
	: timeStep(p_timestep), accumulator(0.0f), currentTime(utils::hireTimeInSeconds()), alpha(0.0f), startTicks(0)
{}
void Timestepper::processFrameStart() {
	startTicks = SDL_GetTicks();
	float newTime = utils::hireTimeInSeconds();
	float frameTime = newTime - currentTime;
	currentTime = newTime;
	accumulator += frameTime;
}
bool Timestepper::accumulatorFull() {
	return accumulator >= timeStep;
}

void Timestepper::calculateAlpha() {
	alpha = accumulator / timeStep;
}
void Timestepper::processFrameEnd(GameWindow& p_window) {
	int frameTicks = SDL_GetTicks() - startTicks;
	if (frameTicks < 1000.0f / p_window.getRefreshRate()) {
		SDL_Delay(1000.0f / p_window.getRefreshRate() - frameTicks);
	}
}