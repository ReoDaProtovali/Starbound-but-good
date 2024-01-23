#pragma once
#include "GameStates.hpp"
#include "imgui.h"

class MenuState : public GameState {
public:
	MenuState() {}

	void init() override;
	void update() override;
	void suspend() override {};
	void resume() override {};
	void close() override;

private:

};