#pragma once
#include "Framework/Graphics/TransformObject.hpp"
#include "CollisionObject.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/DrawStates.hpp"
#include <chrono>
#include "GameConstants.hpp"
#include "Camera.hpp"
// An entity is basically anything that is able to move.
// This class is managed by the server, and does not contain draw code.
class Entity : public TransformObject, public CollisionObject {
public:
	Entity() {};
	virtual ~Entity() {};


	virtual void onSpawn(b2World& p_world) {};
	virtual void onAI() {};
	virtual void onUpdate() {};
	virtual void onDestroy(b2World& p_world) {};
	virtual void draw(DrawSurface& p_target, DrawStates& p_states) {};

	void updateTimestamp() {
		lastUpdateTimestamp = std::chrono::high_resolution_clock::now();
	}
	void getDeltaSinceUpdate() {
		deltaTimeSinceUpdate = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - lastUpdateTimestamp);
	}

	std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTimestamp;
	std::chrono::microseconds deltaTimeSinceUpdate;
	
	bool isPlayer = false;
	Camera myCam;
};


struct EntityWrapper {
	EntityWrapper(Entity* e) : m_eptr(e) {};
	~EntityWrapper() {
		if (m_eptr)
		delete m_eptr;
		m_eptr = nullptr;
	}

	Entity* operator->() {
		return m_eptr;
	}
	Entity* m_eptr = nullptr;
};

//std::ostream& operator<<(std::ostream& os, const EntityWrapper& e)
//{
//	os << e.m_ptr;
//	return os;
//}
