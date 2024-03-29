#include "Game States/ServerWorldState.hpp"
#include "Framework/Log.hpp"
void ServerWorldState::init()
{
	LOAD_LOG("World Server State Started");
	world = new ChunkManager(); // i really don't like doing this, but it has to be made on the right thread
	sim = new Simulation();
	sim->init();
	world->setCollisionWorld(sim->getCollisionWorldPtr());

	world->genFixed(-1, -1, 2, 2);

	world->startThreads();
}

void ServerWorldState::update()
{

	while (auto k = keyObserver.observe()) {
		if (k.value().keyCode == SDLK_4) world->regenVBOs();
		if (k.value().keyCode == SDLK_n) {
			world->removeChunk({ -1, -1 });
			world->genFixed(-1, -1, 1, 1);
		};
	}
	world->processRequests();
	world->tidyNoisemapIfDone();
	world->generateColliders();
	sim->tick();
}
void ServerWorldState::suspend() 
{
	
}
void ServerWorldState::resume()
{
}
void ServerWorldState::close() 
{
	LOG("Server world state closing.");
	keyObserver.unsubscribe();
	world->stopThreads();
	//world->~ChunkManager();
	delete world;
	//sim->~Simulation();
	delete sim;
}