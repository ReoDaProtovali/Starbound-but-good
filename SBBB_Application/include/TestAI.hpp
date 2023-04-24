#pragma once
#include "AI.hpp"
#include "GameConstants.hpp"
#include "Entity.hpp"

class TestAI : public AI {
public:
	TestAI() = delete;
	TestAI(Entity* boundEntity) {
		m_host = boundEntity;
		controller.holdInput = true;
		state[0] = float(rand() % 30);
	}
	void update() override {
		assert(m_host);
		state[0] -= 0.5f;
		if (state[0] <= 0) {
			m_host->applyImpulse(glm::vec2(0.f, jumpStrength));
			state[0] = 30.f + float(rand() % 20);
		}
		switch (rand() % 20) {
		case 1:
			controller.dpadControl(CardinalDirection::LEFT, bool(rand() % 2));
			break;
		case 2: // fall faster, why not
			controller.dpadControl(CardinalDirection::DOWN, bool(rand() % 2));
			break;
		case 3:
			controller.dpadControl(CardinalDirection::RIGHT, bool(rand() % 2));
			break;
		}
		m_host->applyForce(controller.getMovementDir() * moveSpeed);
	}

private:
	const float jumpStrength = 5.0f; // in newtons
	const float moveSpeed = 2.0f; // also in newtons lol
	Entity* m_host = nullptr;
};