#include "GameServer.hpp"
#include "GameConstants.hpp"
#include "Simulation.hpp"

void GameServer::start(SharedQueue<std::exception_ptr>& p_exceptionQueue) {
	serverThread = std::thread(&GameServer::run, this, std::ref(p_exceptionQueue));
}

void GameServer::stop() {
	m_stopping = true;
	serverThread.join();
}
void GameServer::run(SharedQueue<std::exception_ptr>& p_exceptionQueue) {
	serverWindow.create("shouldn't be visible", 100, 100, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	serverWindow.bindToThisThread();

	ResourceManager& res = ResourceManager::Get();
	res.loadGeneratorShaders();

	Observer<MouseEvent> mouseObserver;
	// this kinda has to be outside of the try...catch so it is scoped in the catch block
	ChunkManager world;
	Simulation sim;
	sim.init();
	world.setCollisionWorld(sim.getCollisionWorldPtr());

	world.genFixed(-5, -3, 20, 10);
	world.startThreads();
	try {

		while (true) {

			ts.processFrameStart();
			while (ts.accumulatorFull()) {
				ts.drain();

				world.processRequests();
				world.tidyNoisemapIfDone();
				world.generateColliders();
				

				sim.tick();
				//std::cin.get();

				tickGauge.update(0.9f);

				globals.updateFPS = (float)(1.0 / tickGauge.getFrametimeAverage());

				ts.processFrameStart();
			}
			// prevent it from overworking
			std::this_thread::sleep_for(std::chrono::microseconds(1000000 / (UPDATE_RATE_FPS * 2)));
			if (m_stopping) break;
		}
		world.stopThreads();
		serverWindow.cleanUp();
	}
	catch (std::exception& ex) {
		ERROR_LOG("Exception in " << __FILE__ << " at " << __LINE__ << ": " << ex.what());
		world.stopThreads();
		serverWindow.cleanUp();
		p_exceptionQueue.push(std::current_exception());
	}
}