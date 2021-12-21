#pragma once

#ifndef TIMESTEPPER_H
#define TIMESTEPPER_H

#include "utils.hpp"
#include "SDL.h"
#include "GameWindow.hpp"

// i hate this class and i dont know why it works
struct Timestepper {
	Timestepper(int p_gameUpdateFPS, int p_renderFPS);
	void processFrameStart();
	bool accumulatorFull();
	void calculateAlpha();
	void processFrameEnd();
	float accumulator;
	int gameUpdateFPS;
	int renderFPS;
	float frameTime;
	float alpha;
	fpsGauge fg;
	Uint32 startTicks;
	Uint32 frameTicks;
	float currentTime;
	// the variable alpha is a number between 0-1 that represents the progress percentage towards the next game loop update
	// for example if the window is being rendered at 120fps and the game is updating at 60fps and you're between
	// physics updates, it will be 0.5. It is used for movement interpolation for higher refresh rates I guess
};

#endif