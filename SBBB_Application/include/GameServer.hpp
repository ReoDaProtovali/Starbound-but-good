#pragma once
#include <iostream>
#include <thread>

class GameServer
{
	void run();

	std::jthread serverThread;
	std::atomic_bool m_stopping = false;

public:
	GameServer() {};
	void start();
	void stop();
};

