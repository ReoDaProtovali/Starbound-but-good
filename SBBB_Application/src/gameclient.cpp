#include "GameClient.hpp"

void GameClient::start() {
	clientThread = std::thread(&GameClient::run, this);
}

void GameClient::stop() {
	m_stopping = true;
	clientThread.join();
}
void GameClient::run() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Game running! " << "\n";

		if (m_stopping) break;
	}
}