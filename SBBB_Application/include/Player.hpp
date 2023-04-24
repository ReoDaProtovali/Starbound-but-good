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

		myCam.pos.z = 32.f;

		if (Globals::shouldInterpolate()) {
			myCam.enableInterpolation();
			m_sprite.enableTransformInterpolation();
			myCam.apparentPos = myCam.pos;
			m_sprite.apparentPos = m_position;
			m_sprite.apparentRot = m_rotation;
		}
	}

	void onSpawn(b2World& p_world) {
		setColliderStartPosition(glm::vec2(m_position.x, m_position.y));

		setColliderAsDynamic();
		setDensity(1.f);
		allowColliderRotation(true);

		//setColliderShapeAsBox(m_entityDims.x / 2.f, m_entityDims.y);
		setColliderShapeAsCircle(1.5f);
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
		setRotation(transform.q.GetAngle());

		const float INTERP_FACTOR = (float(UPDATE_RATE_FPS) / float(globals.refresh_rate));
		const float anticipationScale = 5.f * METERS_TO_TILES * (1.f / UPDATE_RATE_FPS);
		const float anticipationSpeed = INTERP_FACTOR;
		glm::vec3 newPos = utils::lerp(myCam.pos, glm::vec3(m_position.x + getVelocity().x * anticipationScale, m_position.y + getVelocity().y * anticipationScale, myCam.pos.z), anticipationSpeed);
		myCam.pos = newPos;

		//if (m_body->GetLinearVelocity().x < 0) {
		//	setScale(glm::vec2(-1.f, 1.f));
		//}
		//else {
		//	setScale(glm::vec2(1.f, 1.f));
		//}
		updateTimestamp();
	}
	void draw(DrawSurface& p_target, DrawStates& p_states) {
		if (m_sprite.bounds.xy.x == 0) { // scuffed init checking
			m_sprite.setBounds(Rect(0.f, 0.f, m_entityDims.x + 0.03f, m_entityDims.y + 0.03f));
			m_sprite.attachTexture(ResourceManager::Get().getTexture(TextureID::REO_TEST));
		}

		m_sprite.cloneTransform(*this);

		if (Globals::shouldInterpolate()) {
			// do the smoothing
			const float INTERP_FACTOR = (float(UPDATE_RATE_FPS) / float(globals.refresh_rate));
			myCam.apparentPos = utils::lerp(myCam.apparentPos, myCam.pos, INTERP_FACTOR);
			m_sprite.apparentPos = utils::lerp(m_sprite.apparentPos, getPosition(), INTERP_FACTOR);
			// a little bit of a silly solution
			double angleDiff = fmod((double)getRotation() - (double)m_sprite.apparentRot + 3.0 * M_PI, 2.0 * M_PI) - M_PI;
			double angle = m_sprite.apparentRot + INTERP_FACTOR * angleDiff;
			angle += (angle < 0) ? 2.f * M_PI : -2.f * M_PI;
			m_sprite.apparentRot = angle;
		}

		m_sprite.setOriginRelative(OriginLoc::CENTER);
		m_sprite.draw(p_target, p_states);

	}

private:
	Sprite m_sprite;
	GenericShaders& gs = GenericShaders::Get();
	glm::vec2 m_entityDims = glm::vec2(0);
	PlayerAI m_AI{ this };
};