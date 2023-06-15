#pragma once 

#include "Entity.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include "PlayerAI.hpp"
#include "ResourceManager.hpp"
#include "util/DebugDraw.hpp"
#include <shared_mutex>

class Player : public Entity {
public:
	Player(float x, float y, float w, float h) {
		isPlayer = true;
		m_entityDims.x = w;
		m_entityDims.y = h;

		setOrigin(Rect(0.f, 0.f, w, h), OriginLoc::CENTER);
		setPosition(glm::vec3(x, y, 5.f));

		m_sprite.setPosition(m_position);

		entityCam.pos.z = 32.f;

		if (Globals::shouldInterpolate()) {
			entityCam.enableInterpolation();

			entityCam.apparentPos = entityCam.pos;
			m_sprite.apparentPos = m_position;
			m_sprite.apparentRot = m_rotation;
		}
	}

	void onSpawn(b2World& p_world);

	void onDestroy(b2World& p_world) {
		p_world.DestroyBody(m_body);
	}

	void onUpdate();
	void draw(DrawSurface& p_target, DrawStates& p_states);

private:
	void drawRayCast(RayCastOutput& out, DrawSurface& p_target, DrawStates& p_states);
	Sprite m_sprite;
	std::shared_mutex m_posMut;
	GenericShaders& gs = GenericShaders::Get();
	glm::vec2 m_entityDims = glm::vec2(0);
	PlayerAI m_AI{ this };
};