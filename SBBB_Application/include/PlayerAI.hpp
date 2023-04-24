#pragma once
#include "AI.hpp"
#include "util/SubjectObserver.hpp"
#include "GameConstants.hpp"
#include "Entity.hpp"
#include "SDL_keycode.h"
class PlayerAI : public AI {
public:
	PlayerAI() = delete;
	PlayerAI(Entity* boundEntity) {
		m_host = boundEntity;
		// to make it controllable via keyup and keydown events
		controller.holdInput = true;
	}
	void update() override {
		assert(m_host);
		while (auto e = m_keyObserver.observe()) {
			KeyEvent event = e.value();
			// we don't actually want to use the controller for up, as that would be a lil weird for a player to just start rising
			switch (event.keyCode) {
			case SDLK_w:
				if (event.wasDown)
					m_host->applyImpulse(b2Vec2(0.f, jumpStrength));
				break;
			case SDLK_a:
				controller.dpadControl(CardinalDirection::LEFT, event.wasDown);
				break;
			case SDLK_s: // fall faster, why not
				controller.dpadControl(CardinalDirection::DOWN, event.wasDown);
				break;
			case SDLK_d:
				controller.dpadControl(CardinalDirection::RIGHT, event.wasDown);
				break;
			}
		}
		m_host->applyForce(controller.getMovementDir() * moveSpeed);
	}

private:
	const float jumpStrength = 20.0f; // in newtons
	const float moveSpeed = 30.0f; // also in newtons lol
	Entity* m_host = nullptr;
	Observer<KeyEvent> m_keyObserver;
};