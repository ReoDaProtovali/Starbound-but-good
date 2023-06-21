#pragma once

#include "GameStates.hpp"
#include "ChunkManager.hpp"
#include "Simulation.hpp"

class ServerWorldState : public GameState {
public:
	ServerWorldState() {}

	void init() override;
	void update() override;
	void suspend() override;
	void resume() override;
	void close() override;

private:
	ChunkManager* world;
	Simulation* sim;
	Observer<KeyEvent> keyObserver{globals.keySubject};
};