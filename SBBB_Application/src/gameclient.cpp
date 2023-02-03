#include "GameClient.hpp"

GameClient::GameClient() {

}

void GameClient::start() {

}
void GameClient::run() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Game running! '\n";

		if (m_stopping) break;
	}
}