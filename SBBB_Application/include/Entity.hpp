#pragma once
#include "Framework/Graphics/TransformObject.hpp"
#include "CollisionObject.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/DrawStates.hpp"

// An entity is basically anything that is able to move.
// This class is managed by the server, and does not contain draw code.
class Entity : public TransformObject, public CollisionObject {
public:
	Entity() {};

	virtual void onSpawn(b2World& p_world) {};
	virtual void onAI() {};
	virtual void onUpdate() {};
	virtual void onDestroy(b2World& p_world) {};
	virtual void draw(DrawSurface& p_target, DrawStates& p_states) {};
};