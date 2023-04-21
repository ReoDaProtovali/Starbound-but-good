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
		m_collisionDims.x = w;
		m_collisionDims.y = h;

		setOrigin(Rect(0.f, 0.f, w, h), OriginLoc::CENTER);
		setPosition(glm::vec3(x, y, 5.f));

	}

	void onSpawn(b2World& p_world) {
		setColliderStartPosition(glm::vec2(m_position.x, m_position.y));

		setColliderAsDynamic();
		setDensity(1.f);
		allowColliderRotation(false);

		setColliderShapeAsBox(m_collisionDims.x, m_collisionDims.y);
		setFriction(0.2f);
		setRestitution(0.1f);

		m_def.awake = true;
		m_def.allowSleep = false;

		spawnCollider(p_world);
	}

	void onDestroy(b2World& p_world) {
		p_world.DestroyBody(m_body);
	}

	void onUpdate() {
		m_AI.update();
		auto transform = getCollisionTransform();
		m_position.x = transform.p.x;
		m_position.y = transform.p.y;
		setRotation(transform.q.GetAngle());

	}
	void draw(DrawSurface& p_target, DrawStates& p_states) {
		if (m_sprite.bounds.xy.x == 0) { // scuffed init checking
			m_sprite.setBounds(Rect(0.f, 0.f, m_collisionDims.x + 0.03f, m_collisionDims.y + 0.03f));
			m_sprite.attachTexture(ResourceManager::Get().getTexture(TextureID::REO_TEST));
		}

		m_sprite.cloneTransform(*this);
		m_sprite.setOriginRelative(OriginLoc::CENTER);
		m_sprite.draw(p_target, p_states);

	}

	glm::vec2 getSpritePos() { m_sprite.calculateTransform(); return glm::vec2(m_sprite.getPosition().x, m_sprite.getPosition().y); }
private:
	Sprite m_sprite;
	GenericShaders& gs = GenericShaders::Get();
	glm::vec2 m_collisionDims = glm::vec2(0);
	PlayerAI m_AI{ this };
};