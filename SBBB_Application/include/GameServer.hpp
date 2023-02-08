#pragma once
#include <iostream>
#include <thread>
#include "util/utils.hpp"
#include "Timestepper.hpp"
#include "ChunkManager.hpp"
#include "Framework/Window/GameWindow.hpp"

class GameServer
{
	void run();

	Timestepper ts {UPDATE_RATE_FPS};
	
	std::exception_ptr except;
	fpsGauge tickGauge;
	std::thread serverThread;
	std::atomic_bool m_stopping = false;

	// offscreen rendering on server thread
	GameWindow serverWindow;
public:
	GameServer() {};
	void start();
	void stop();
};

