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
	void update() override;

private:
	const float jumpStrength = 20.0f; // in newtons
	const float moveSpeed = 30.0f; // also in newtons lol
	Entity* m_host = nullptr;
	Observer<KeyEvent> m_keyObserver{ globals.keySubject };
};