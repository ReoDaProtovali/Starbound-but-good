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

	//fpsGauge updateFPSGauge;
	//fpsGauge renderFPSGauge;

	//ChunkManager world;

	//Timestepper ts = Timestepper(GAME_UPDATE_SPEED);

	//int printConsoleCounter = 0; // to limit the amount the console updates as to not cause lag
	//int lastChunkDrawnCount = 0;

	//uint32_t renderFrame = 0;
	//uint32_t updateFrame = 0;

	bool gameActive = true;

	void startClient();
	void startServer();
	void pollEvents();
	void handleInput();
	void processConsoleStats();

public:
	Application();

	void run();
	void cleanUp();

};