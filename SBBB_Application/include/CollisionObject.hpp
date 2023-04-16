#pragma once
#include "box2d.h"
#include "glm/glm.hpp"
#include "util/Rect.hpp"

// A collision object is a simple single-fixture object that can be either dynamic or static. 
class CollisionObject {
public:
	CollisionObject() {};
	
	CollisionObject(CollisionObject& other) {
		m_def = other.m_def;
		m_shape = other.m_shape;
		m_fixtureDef = other.m_fixtureDef;
		m_body = other.m_body;
		m_fixtureDef.shape = &m_shape;
	}
	// this stuff isn't working well for me
	CollisionObject& operator=(const CollisionObject& other) {
		m_def = other.m_def;
		m_fixtureDef = other.m_fixtureDef;
		m_shape = other.m_shape;
		m_fixtureDef.shape = &m_shape; // oh.
		m_body = other.m_body;
		return *this;
	}
	CollisionObject& operator=(CollisionObject&& other) noexcept {
		m_def = other.m_def;
		m_fixtureDef = other.m_fixtureDef;
		m_shape = other.m_shape;
		m_fixtureDef.shape = &m_shape;
		m_body = other.m_body;
		return *this;
	}
	// might seem a bit silly to have all of these as seperate functions but it provides some level of intuition
	// yes these names are long but we have monitor space so why not use it
	void setColliderBodyDef(const b2BodyDef& p_def);
	void allowColliderRotation(bool p_enabled);
	void setColliderStartPosition(const glm::vec2& p_pos);
	void setColliderAsStatic();
	void setColliderAsDynamic();
	void setColliderShapeAsBox(float p_w, float p_h);

	// aaand we gotta do this again for the fixtures because I wanna
	void setColliderFixtureDef(const b2FixtureDef& p_def);
	void setFriction(float p_friction);
	void setRestitution(float p_restitution);
	void setRestitutionThreshold(float p_restitutionThreshold);
	void setDensity(float p_density);

	// Must be done after proper definitions
	void spawnCollider(b2World& p_world);
	// deletes first, then respawns
	void respawnCollider(b2World& p_world);

	const b2Transform& getCollisionTransform();
protected:
	// used for setup
	b2BodyDef m_def{};
	b2FixtureDef m_fixtureDef{};
	b2PolygonShape m_shape{};

	// pointer to our body within the world
	b2Body* m_body = nullptr;
};