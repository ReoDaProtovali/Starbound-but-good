#pragma once
#include <iostream>
#include <thread>
#include "util/utils.hpp"
#include "Timestepper.hpp"
#include "ChunkManager.hpp"
#include "Framework/Window/GameWindow.hpp"
#include "util/Messenger.hpp"
#include "util/SharedQueue.hpp"
#include "GameStates.hpp"
#include "Game States/ServerWorldState.hpp"
#include "Game States/TemplateState.hpp"
class GameServer
{
	void run(SharedQueue<std::exception_ptr>& p_exceptionQueue);

	Timestepper ts {UPDATE_RATE_FPS};
	
	fpsGauge tickGauge;

	GameStateManager& stateManager = GameStateManager::Get();
	ServerWorldState State_ServerWorld;
	TemplateState State_None;

	// thread management
	std::thread serverThread;

	// offscreen rendering on server thread
	GameWindow serverWindow;
public:
	GameServer();
	void start(SharedQueue<std::exception_ptr>& p_exceptionQueue);
	void stop();
	std::atomic_bool stopping = false;
};

