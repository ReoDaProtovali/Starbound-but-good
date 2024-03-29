#pragma once
#include "box2d.h"
#include "glm/glm.hpp"
#include "util/Rect.hpp"
#include <atomic>

struct RayCastOutput {
	glm::vec2 rayStart = glm::vec2(0.f);
	glm::vec2 point = glm::vec2(0.f);
	glm::vec2 normal = glm::vec2(0.f);
	bool hit = false;
};
struct RayCastCallback : public b2RayCastCallback {
	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
		out.point = glm::vec2(point.x, point.y);
		out.normal = glm::vec2(normal.x, normal.y);
		out.hit = true;
		return fraction;
	}
	RayCastOutput out;
};

// A collision object is a simple single-fixture object that can be either dynamic or static. 
class CollisionObject {
public:
	CollisionObject() {};
	
	CollisionObject(CollisionObject& other) {
		m_def = other.m_def;
		m_polygonShape = other.m_polygonShape;
		m_fixtureDef = other.m_fixtureDef;
		m_circleShape = other.m_circleShape;
		if (isCircle) {
			m_fixtureDef.shape = &m_circleShape;
		}
		else {
			m_fixtureDef.shape = &m_polygonShape;
		}
		m_body = other.m_body;
		m_fixtureDef.shape = &m_polygonShape;
	}
	// this stuff isn't working well for me
	CollisionObject& operator=(const CollisionObject& other) {
		m_def = other.m_def;
		m_fixtureDef = other.m_fixtureDef;
		m_polygonShape = other.m_polygonShape;
		m_circleShape = other.m_circleShape;
		isCircle = other.isCircle;
		if (isCircle) {
			m_fixtureDef.shape = &m_circleShape;
		}
		else {
			m_fixtureDef.shape = &m_polygonShape;
		}
		m_body = other.m_body;
		return *this;
	}
	CollisionObject& operator=(CollisionObject&& other) noexcept {
		m_def = other.m_def;
		m_fixtureDef = other.m_fixtureDef;
		m_polygonShape = other.m_polygonShape;
		m_circleShape = other.m_circleShape;
		isCircle = other.isCircle;
		if (isCircle) {
			m_fixtureDef.shape = &m_circleShape;
		}
		else {
			m_fixtureDef.shape = &m_polygonShape;
		}
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
	void setColliderShapeAsCircle(float r);

	// aaand we gotta do this again for the fixtures because I wanna
	void setColliderFixtureDef(const b2FixtureDef& p_def);
	void setFriction(float p_friction);
	void setRestitution(float p_restitution);
	void setRestitutionThreshold(float p_restitutionThreshold);
	void setLinearDamping(float p_damping);
	void setDensity(float p_density);
	void teleportColliderRelative(float xOff, float yOff);

	void setVelocity(b2Vec2 p_velocity);
	void applyForce(b2Vec2 p_force);
	void applyImpulse(b2Vec2 p_impulse);
	void applyForce(const glm::vec2& p_force);
	void applyImpulse(const glm::vec2& p_impulse);
	void wake();

	void updateValues();
	// Must be done after proper definitions
	void spawnCollider(b2World& p_world);
	// deletes first, then respawns
	void respawnCollider(b2World& p_world);

	b2Transform getCollisionTransform();
	b2Transform getCollisionTransformMeters();
	glm::vec2 getVelocity();

	// units are in tiles, not meters.
	RayCastOutput rayCastRelative(float offX, float offY, float endOffX, float endOffY);

	std::atomic<glm::vec2> bodyVelocity;
protected:
	// used for setup
	b2BodyDef m_def{};
	b2FixtureDef m_fixtureDef{};
	b2PolygonShape m_polygonShape{};
	b2CircleShape m_circleShape;
	bool isCircle = false;

	// pointer to our body within the world
	b2Body* m_body = nullptr;
};