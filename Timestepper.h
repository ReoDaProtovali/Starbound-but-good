#pragma once

#ifndef TIMESTEPPER_H
#define TIMESTEPPER_H

#include "utils.h"
#include "SDL.h"
#include "GameWindow.hpp"

class Timestepper {
public:
	Timestepper(float p_timestep);
	void processFrameStart();
	bool accumulatorFull();
	void calculateAlpha();
	void processFrameEnd(GameWindow& p_window);
	float accumulator;
	float timeStep;
	float alpha;
	// the variable alpha is a number between 0-1 that represents the progress percentage towards the next game loop update
	// for example if the window is being rendered at 120fps and the game is updating at 60fps and you're between
	// physics updates, it will be 0.5. It is used for movement interpolation for higher refresh rates I guess
private:
	int startTicks;
	float currentTime;
};

#endif