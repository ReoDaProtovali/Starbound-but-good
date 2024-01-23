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
#include "Game States/MenuState.hpp"
#include "Game States/TemplateState.hpp"

class GameClient
{
	void run(SharedQueue<std::exception_ptr>& p_exceptionQueue);
	void testInput();
	void processDebugStats();
	void resizeWindow(uint32_t p_w, uint32_t p_h);
	// core systems
public: GameWindow gw { "Barstound" };
private: GameRenderer renderer{ gw };

	GUI& gui = GUI::Get();

	GameStateManager& stateManager = GameStateManager::Get();
	ClientWorldState State_ClientWorld{ gw, renderer };
	MenuState State_Menu{};
	TemplateState State_None;

	// utility/testing
	glm::vec2 camVelocity = glm::vec2(0.0f, 0.0f);
	fpsGauge renderFPSGauge;

	// thread management
	std::thread clientThread;
	BidirectionalMessenger<ChunkPos, int>& s_generationRequest = BidirectionalMessenger<ChunkPos, int>::Get();

	void cleanUp();

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
	ImGuiContext* imctx = nullptr;
	std::mutex inputReadWriteMutex;
	std::atomic<bool> flagResize = false;
	std::atomic_bool clientStopping = false;

};

