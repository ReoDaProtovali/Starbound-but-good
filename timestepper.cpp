#include "Timestepper.hpp"

Timestepper::Timestepper(int p_gameUpdateFPS, int p_renderFPS)
	: renderFPS(p_renderFPS), gameUpdateFPS(p_gameUpdateFPS), accumulator(0.0f), currentTime(utils::hireTimeInSeconds()), alpha(0.0f), startTicks(0), frameTicks(0), frameTime(0.0f)
{}
void Timestepper::processFrameStart() {
	fg.stopStopwatch();
	fg.startStopwatch();
	accumulator += fg.getSecondsElapsed();
}
bool Timestepper::accumulatorFull() {
	return accumulator >= 1.0f / gameUpdateFPS;
}

void Timestepper::calculateAlpha() {
	alpha = accumulator * gameUpdateFPS; // did some quick maf, same thing as accumulator / (1 / gameUpdateFPS)
}
void Timestepper::processFrameEnd() {

}