#pragma once
#include "MovementController.hpp"
class AI {
public:
	virtual void update() {};

	void setMovementController(const MovementController& p_controller) {
		controller = p_controller;
	}
protected:
	float state[32] = {0}; // 32 state variables for now
	MovementController controller;
};