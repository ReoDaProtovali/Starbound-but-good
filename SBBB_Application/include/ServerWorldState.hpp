#pragma once

#include "GameStates.hpp"
#include "ChunkManager.hpp"
#include "Simulation.hpp"

class ServerWorldState : public GameState {
public:
	ServerWorldState() {}

	void init() override {
		world = new ChunkManager(); // i really don't like doing this, but it has to be made on the right thread
		sim = new Simulation();
		sim->init();
		world->setCollisionWorld(sim->getCollisionWorldPtr());

		world->genFixed(-5, -3, 20, 10);
		world->startThreads();
	}
	void update() override {
		world->processRequests();
		world->tidyNoisemapIfDone();
		world->generateColliders();
		sim->tick();
	}
	void suspend() override {

	}
	void close() override {
		world->stopThreads();

		delete world;
		delete sim;
	}

private:
	ChunkManager* world;
	Simulation* sim;
};