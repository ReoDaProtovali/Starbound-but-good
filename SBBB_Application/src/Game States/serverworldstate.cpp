#include "Game States/ServerWorldState.hpp"
#include "Framework/Log.hpp"
void ServerWorldState::init()
{
	LOAD_LOG("World Server State Started");
	world = new ChunkManager(); // i really don't like doing this, but it has to be made on the right thread
	sim = new Simulation();
	sim->init();
	world->setCollisionWorld(sim->getCollisionWorldPtr());

	world->genFixed(-10, -6, 20, 10);
	world->startThreads();
}

void ServerWorldState::update()
{
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
	world->stopThreads();
	delete world;
	delete sim;
}