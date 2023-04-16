#pragma once
#include "Framework/Graphics/TransformObject.hpp"
#include "CollisionObject.hpp"
// An entity is basically anything that is able to move.
// This class is managed by the server, and does not contain draw code.
class Entity : public TransformObject, public CollisionObject {
public:
	Entity() {
		
	};

	Entity& operator=(const Entity& other) {
		cloneTransform(other);
		m_def = other.m_def;
		m_fixtureDef = other.m_fixtureDef;
		m_shape = b2PolygonShape();
		m_shape.SetAsBox(1.f, 1.f);
		m_fixtureDef.shape = &m_shape;
		m_body = other.m_body;
	}
	virtual void onSpawn(b2World& p_world) {};
	virtual void onAI() {};
	virtual void onUpdate() {};
	virtual void onDestroy(b2World& p_world) {};
	
};