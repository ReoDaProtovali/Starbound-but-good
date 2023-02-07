#pragma once
#include <iostream>
#include <thread>
#include "Framework/Window/GameWindow.hpp"
#include "GameRenderer.hpp"

class GameClient
{
	void run();
	// core systems
	GameWindow gw{ "Barstound" };
	GameRenderer renderer{ gw };
	InputHandler inp;

	// utility/testing
	glm::vec2 camVelocity = glm::vec2(0.0f, 0.0f);
	fpsGauge renderFPSGauge;

	// thread management
	std::jthread clientThread;
	std::atomic_bool m_stopping = false;

public:
	GameClient();
	~GameClient();
	void start();
	void stop();
};

