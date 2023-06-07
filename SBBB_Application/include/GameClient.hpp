#pragma once
#include <iostream>
#include <thread>
#include "Framework/Window/GameWindow.hpp"
#include "GameRenderer.hpp"
#include "GUI.hpp"
#include "util/Messenger.hpp"
#include "util/SharedQueue.hpp"
#include "GameStates.hpp"

#include "Game States/ClientWorldState.hpp"
#include "Game States/TemplateState.hpp"

class GameClient
{
	void run(SharedQueue<std::exception_ptr>& p_exceptionQueue);
	void testInput();
	void processDebugStats();
	void resizeWindow(uint32_t p_w, uint32_t p_h);
	// core systems
	GameWindow gw { "Barstound" };
	GameRenderer renderer{ gw };

	GameStateManager& stateManager = GameStateManager::Get();
	ClientWorldState State_ClientWorld{ gw, renderer };
	TemplateState State_None;

	// utility/testing
	glm::vec2 camVelocity = glm::vec2(0.0f, 0.0f);
	fpsGauge renderFPSGauge;

	// thread management
	std::thread clientThread;
	std::atomic_bool m_stopping = false;
	Messenger<ChunkPos, int>& s_generationRequest = Messenger<ChunkPos, int>::Get();


public:
	GameClient();
	~GameClient();
	void start(SharedQueue<std::exception_ptr>& p_exceptionQueue);
	void stop();
	uint32_t getWindowID() { return gw.getWindowID(); }

	// for access by the main thread
	uint32_t newWidth = 0;
	uint32_t newHeight = 0;
	InputHandler inp;
	std::mutex inputReadWriteMutex;
	std::atomic<bool> flagResize = false;

};

