#pragma once
#include <box2d.h>
#include "util/SharedDynArray.hpp"
#include "TestEntity.hpp"
// Primary game logic, entities, collision and stuff
class Simulation {
public:
	Simulation() {};
	~Simulation() { for (auto e : allocedEntities) delete e; }
	void init();

	void spawnTestEntities();

	void tick();

	b2World* getCollisionWorldPtr() { return &m_physicsWorld; }
private:
	b2World m_physicsWorld{b2Vec2(0.f, -10.f)};
	std::vector<Entity*> allocedEntities;
	SharedDynArray<Entity*>& m_entities = SharedDynArray<Entity*>::Get();
};