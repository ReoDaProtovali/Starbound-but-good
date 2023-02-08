#pragma once
#include <iostream>
#include <thread>
#include "Framework/Window/GameWindow.hpp"
#include "GameRenderer.hpp"
#include "util/Messenger.hpp"

class GameClient
{
	void run();
	void render();
	void testInput();
	void processDebugStats();
	void resizeWindow(uint32_t p_w, uint32_t p_h);
	// core systems
	GameWindow gw { "Barstound" };
	GameRenderer renderer{ gw };

	// utility/testing
	glm::vec2 camVelocity = glm::vec2(0.0f, 0.0f);
	fpsGauge renderFPSGauge;

	// thread management
	std::thread clientThread;
	std::atomic_bool m_stopping = false;
	Messenger<ChunkPos, WorldChunk*>& m_chunkMessenger = Messenger<ChunkPos, WorldChunk*>::Get();


public:
	GameClient();
	~GameClient();
	void start();
	void stop();
	uint32_t getWindowID() { return gw.getWindowID(); }

	// for access by the main thread
	uint32_t newWidth = 0;
	uint32_t newHeight = 0;
	InputHandler inp;
	std::mutex inputReadWriteMutex;
	std::atomic<bool> flagResize = false;

};

