#include "GameServer.hpp"

void GameServer::start() {
	serverThread = std::jthread(&GameServer::run, this);
}

void GameServer::stop() {
	m_stopping = true;
	printf("%p\n", &m_stopping);
	serverThread.join();
}
void GameServer::run() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Server running! " << "\n";
		printf("%p\n", &m_stopping);
		if (m_stopping) break;
	}
}