#pragma once
#include "Entity.hpp"
#include "Framework/Graphics/TransformObject.hpp"
#include "CollisionObject.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "ResourceManager.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include "TestAI.hpp"
class TestEntity : public Entity {
public:
	TestEntity(float x, float y, float w, float h, bool stationary) {
		
		m_entityDims.x = w;
		m_entityDims.y = h;
		m_position.x = x;
		m_position.y = y;
		m_position.z = 5.f;
		//m_sprite.attachShader(&gs.solidColorShader);

		m_sprite.enableTransformInterpolation();
		setOrigin(Rect(0.f, 0.f, w, h), OriginLoc::CENTER);
		m_stationary = stationary;
	}

	//TestEntity& operator=(const TestEntity& other) = default;
	TestEntity& operator=(const TestEntity& other) {
		m_sprite = other.m_sprite;
		m_entityDims = other.m_entityDims;
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

		setColliderShapeAsBox(m_entityDims.x, m_entityDims.y);
		setFriction(0.2f);
		setRestitution(0.1f);

		//m_def.awake = true;
		//m_def.allowSleep = false;

		spawnCollider(p_world);
	}

	void onDestroy(b2World& p_world) {
		p_world.DestroyBody(m_body);
	}
	void onUpdate() {
		//m_AI.update();
		auto transform = getCollisionTransform();
		m_position.x = transform.p.x;
		m_position.y = transform.p.y;
		setRotation(transform.q.GetAngle());
		m_sprite.cloneTransform(*this);
	}
	void draw(DrawSurface& p_target, DrawStates& p_states) {
		if (m_sprite.bounds.xy.x == 0) { // scuffed init checking
			m_sprite.setBounds(Rect(0.f, 0.f, m_entityDims.x + 0.03f, m_entityDims.y + 0.03f));
			m_sprite.attachTexture(ResourceManager::Get().getTexture(TextureID::ME_TEXTURE));

		}

		if (Globals::shouldInterpolate()) {
			// do the smoothing
			const float INTERP_FACTOR = (float(UPDATE_RATE_FPS) / float(globals.refresh_rate)) / 2.f;

			m_sprite.apparentPos = utils::lerp(m_sprite.apparentPos, getPosition(), INTERP_FACTOR);

			// a little bit of a silly solution
			double angleDiff = fmod((double)getRotation() - (double)m_sprite.apparentRot + 3.0 * M_PI, 2.0 * M_PI) - M_PI;
			double angle = m_sprite.apparentRot + INTERP_FACTOR * angleDiff;
			angle += (angle < 0) ? 2.f * M_PI : -2.f * M_PI;
			m_sprite.apparentRot = (float)angle;
		}

		m_sprite.setOriginRelative(OriginLoc::CENTER);
		m_sprite.draw(p_target, p_states);
	}

private:
	TestAI m_AI{ this };
	Sprite m_sprite;
	GenericShaders& gs = GenericShaders::Get();
	glm::vec2 m_entityDims = glm::vec2(0);
	bool m_stationary = false;

};