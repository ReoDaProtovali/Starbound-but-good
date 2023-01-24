#pragma once

#include <SDL.h>
#include "Framework/Window/GameWindow.hpp"
#include "GameRenderer.hpp"
#include "GameConstants.hpp"
#include <cmath>

#include "ChunkManager.hpp"
#include "Timestepper.hpp"
#include "Framework/Input/InputHandler.hpp"


class Application {
	
	fpsGauge updateFPSGauge;
	fpsGauge renderFPSGauge;

	GameWindow& gw;
	GameRenderer renderer;

	glm::vec2 camVelocity = glm::vec2(0.0f, 0.0f);

	InputHandler inp;

	ChunkManager world;

	Timestepper ts = Timestepper(GAME_UPDATE_SPEED);

	int printConsoleCounter = 0; // to limit the amount the console updates as to not cause lag
	int lastChunkDrawnCount = 0;

	uint32_t renderFrame = 0;
	uint32_t updateFrame = 0;

	bool gameActive = false;

	void render();
	void update();
	void pollEvents();
	void handleInput();
	void resizeWindow(uint16_t p_w, uint16_t p_h);
	void processConsoleStats();

public:
	Application(GameWindow& p_window);

	void run();
	void cleanUp();

};