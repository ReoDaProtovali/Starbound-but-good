#pragma once
#include "Entity.hpp"
#include "Framework/Graphics/TransformObject.hpp"
#include "CollisionObject.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "ResourceManager.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
class TestEntity : public Entity {
public:
	TestEntity(float x, float y, float w, float h, bool stationary) {
		
		m_collisionDims.x = w;
		m_collisionDims.y = h;

		//m_sprite.attachShader(&gs.solidColorShader);

		setOrigin(Rect(0.f, 0.f, w, h), OriginLoc::CENTER);
		setPosition(glm::vec3(x, y, 5.f));

		m_stationary = stationary;
	}

	//TestEntity& operator=(const TestEntity& other) = default;
	TestEntity& operator=(const TestEntity& other) {
		m_sprite = other.m_sprite;
		m_collisionDims = other.m_collisionDims;
		CollisionObject::operator=(other);
		TransformObject::operator=(other);
		return *this;
	}

	void onSpawn(b2World& p_world) {
		setColliderStartPosition(glm::vec2(m_position.x, m_position.y));

		if (m_stationary) {
			setColliderAsStatic();

		}
		else {
			setColliderAsDynamic();
			setDensity(1.f);
			allowColliderRotation(true);
		}

		setColliderShapeAsBox(m_collisionDims.x, m_collisionDims.y);
		setFriction(0.2f);
		setRestitution(0.1f);

		spawnCollider(p_world);
	}

	void onDestroy(b2World& p_world) {
		p_world.DestroyBody(m_body);
	}
	void onUpdate() {
		auto transform = getCollisionTransform();
		m_position.x = transform.p.x;
		m_position.y = transform.p.y;
		setRotation(transform.q.GetAngle());
		m_sprite.cloneTransform(*this);
	}
	void draw(DrawSurface& p_target, DrawStates& p_states) {
		if (m_sprite.bounds.xy.x == 0) { // scuffed init checking
			m_sprite.setBounds(Rect(0.f, 0.f, m_collisionDims.x + 0.03f, m_collisionDims.y + 0.03f));
			m_sprite.attachTexture(ResourceManager::Get().getTexture(TextureID::ME_TEXTURE));

		}
		//ResourceManager& res = ResourceManager::Get();
		//meTexture = res.getTexture(TextureID::ME_TEXTURE);

		m_sprite.setOriginRelative(OriginLoc::CENTER);
		//gs.solidColorShader.setVec3Uniform(1, glm::vec3(1.f, 0.f, 1.f));
		m_sprite.draw(p_target, p_states);
	}

private:
	Sprite m_sprite;
	GenericShaders& gs = GenericShaders::Get();
	glm::vec2 m_collisionDims = glm::vec2(0);
	bool m_stationary = false;

};