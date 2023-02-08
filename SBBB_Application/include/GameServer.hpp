#pragma once
#include <iostream>
#include <thread>
#include "util/utils.hpp"
#include "Timestepper.hpp"
#include "ChunkManager.hpp"
#include "Framework/Window/GameWindow.hpp"
#include "util/Messenger.hpp"
class GameServer
{
	void run();

	Timestepper ts {UPDATE_RATE_FPS};
	
	fpsGauge tickGauge;

	// thread management
	std::thread serverThread;
	std::atomic_bool m_stopping = false;
	Messenger<ChunkPos, WorldChunk*>& m_chunkMessenger = Messenger<ChunkPos, WorldChunk*>::Get();

	// offscreen rendering on server thread
	GameWindow serverWindow;
public:
	GameServer() {};
	void start();
	void stop();
};

