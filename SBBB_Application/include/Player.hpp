#pragma once 

#include "Entity.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include "PlayerAI.hpp"
#include "ResourceManager.hpp"

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

	void onSpawn(b2World& p_world) {
		setColliderStartPosition(glm::vec2(m_position.x, m_position.y));

		setColliderAsDynamic();
		setDensity(1.f);
		allowColliderRotation(false);

		setColliderShapeAsBox(m_entityDims.x / 2.f, m_entityDims.y);
		//setColliderShapeAsCircle(1.5f);
		setFriction(1.0f);

		setRestitution(0.06f);

		m_def.awake = true;
		m_def.allowSleep = false;

		spawnCollider(p_world);
	}

	void onDestroy(b2World& p_world) {
		p_world.DestroyBody(m_body);
	}

	void onUpdate() {
		CollisionObject::updateValues();
		m_AI.update();
		auto transform = getCollisionTransform();
		m_position.x = transform.p.x;
		m_position.y = transform.p.y;
		//setRotation(transform.q.GetAngle());

		const float INTERP_FACTOR = (float(UPDATE_RATE_FPS * 2) / float(globals.refresh_rate));
		const float anticipationScale = 2.f * METERS_TO_TILES * (1.f / UPDATE_RATE_FPS);
		const float anticipationSpeed = INTERP_FACTOR;
		glm::vec3 newPos = utils::lerp(entityCam.pos, glm::vec3(m_position.x + getVelocity().x * anticipationScale, m_position.y + getVelocity().y * anticipationScale, entityCam.pos.z), anticipationSpeed);
		entityCam.pos = newPos;
		//entityCam.pos.x = m_position.x;
		//entityCam.pos.y = m_position.y;

		if (m_body->GetLinearVelocity().x < 0 && fabs(getVelocity().x) > 0.1f) {
			setScale(glm::vec2(-1.f, 1.f));
		}
		if (m_body->GetLinearVelocity().x > 0 && fabs(getVelocity().x) > 0.1f) {
			setScale(glm::vec2(1.f, 1.f));
		}
		updateTimestamp();
	}
	void draw(DrawSurface& p_target, DrawStates& p_states) {
		if (m_sprite.bounds.xy.x == 0) { // scuffed init checking
			m_sprite.setBounds(Rect(0.f, 0.f, m_entityDims.x + 0.03f, m_entityDims.y + 0.03f));
			m_sprite.attachTexture(ResourceManager::Get().getTexture(TextureID::REO_TEST));
		}

		m_sprite.cloneTransform(*this);

		m_sprite.setOriginRelative(OriginLoc::CENTER);
		m_sprite.draw(p_target, p_states);
	}

private:
	Sprite m_sprite;
	GenericShaders& gs = GenericShaders::Get();
	glm::vec2 m_entityDims = glm::vec2(0);
	PlayerAI m_AI{ this };
};