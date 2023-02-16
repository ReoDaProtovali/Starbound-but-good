#pragma once

#include <SDL.h>
#include "Framework/Window/GameWindow.hpp"
#include "GameRenderer.hpp"
#include "GameConstants.hpp"
#include <cmath>

#include "ChunkManager.hpp"
#include "Timestepper.hpp"
#include "Framework/Input/InputHandler.hpp"
#include "GameClient.hpp"
#include "GameServer.hpp"

class Application {

	GameClient client;
	GameServer localServer;

	SharedQueue<std::exception_ptr> m_exceptionQueue;

	bool gameActive = true;

	void startClient();
	void startServer();
	void pollEvents();

public:
	Application();

	void run();
};