#include "GameServer.hpp"

void GameServer::start() {
	serverThread = std::thread(&GameServer::run, this);
}

void GameServer::stop() {
	m_stopping = true;
	serverThread.join();
}
void GameServer::run() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Game running! " << "\n";

		if (m_stopping) break;
	}
}