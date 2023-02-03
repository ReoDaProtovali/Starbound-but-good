#pragma once
#include <iostream>
#include <thread>

class GameClient
{
	void run();

	std::atomic_bool m_stopping = false;
public:
	GameClient();
	void start();
	void stop();
};

