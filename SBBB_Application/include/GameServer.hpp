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
#include "ServerWorldState.hpp"

class GameServer
{
	void run(SharedQueue<std::exception_ptr>& p_exceptionQueue);

	Timestepper ts {UPDATE_RATE_FPS};
	
	fpsGauge tickGauge;

	GameStateManager& stateManager = GameStateManager::Get();
	ServerWorldState State_ServerWorld;

	// thread management
	std::thread serverThread;
	std::atomic_bool m_stopping = false;

	// offscreen rendering on server thread
	GameWindow serverWindow;
public:
	GameServer();
	void start(SharedQueue<std::exception_ptr>& p_exceptionQueue);
	void stop();
};

