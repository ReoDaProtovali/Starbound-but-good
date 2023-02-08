#include "GameServer.hpp"
#include "GameConstants.hpp"

void GameServer::start() {
	serverThread = std::thread(&GameServer::run, this);
}

void GameServer::stop() {
	m_stopping = true;
	serverThread.join();
}
void GameServer::run() {
	serverWindow.create("shouldn't be visible", 100, 100, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	serverWindow.bindToThisThread();
	ChunkManager world;
	ResourceManager& res = ResourceManager::Get();
	// must be done on this thread because the serverside opengl context needs access to the shaders
	res.loadGeneratorShaders();

	world.genFixed(10, 10);
	world.startThreads();

	while (true) {

		ts.processFrameStart();
		while (ts.accumulatorFull()) {
			ts.drain();

			auto msg = m_chunkMessenger.getMessageFront();
			if (msg.has_value()) {
				LOG("Server Got Message! x: " << msg.value().x << " y: " << msg.value().y);
				LOG("Sending a silly pointer!");
				m_chunkMessenger.sendMessageBack((WorldChunk*)0x69696969);
			}
			tickGauge.update(30);

			//if (rand() % 20 == 0) {
			//	LOG("current server fps: " << 1.f / utils::averageVector(tickGauge.frametimeBuffer));
			//}

			ts.processFrameStart();
		}
		// prevent it from overworking
		std::this_thread::sleep_for(std::chrono::microseconds(1000000 / (UPDATE_RATE_FPS * 2)));

		if (m_stopping) break;
	}
	world.stopThreads();
	serverWindow.cleanUp();


}