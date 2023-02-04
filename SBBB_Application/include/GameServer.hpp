#pragma once
#include <iostream>
#include <thread>

class GameServer
{
	void run();

	std::atomic_bool m_stopping = false;
	std::thread serverThread;

public:
	GameServer() {};
	void start();
	void stop();
};

