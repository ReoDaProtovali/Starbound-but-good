#pragma once
#include <box2d.h>
#include "util/SharedDynArray.hpp"
#include "TestEntity.hpp"
#include "util/SharedList.hpp"
// Primary game logic, entities, collision and stuff
class Simulation {
public:
	Simulation() {};
	//~Simulation() { for (auto e : allocedEntities) delete e; }
	void init();

	void spawnTestEntities();

	void tick();

	b2World* getCollisionWorldPtr() { return &m_physicsWorld; }
private:
	b2World m_physicsWorld{b2Vec2(0.f, 0.f)};
	//std::vector<EntityWrapper> allocedEntities;
	SharedList<EntityWrapper>& m_entities = SharedList<EntityWrapper>::Get();
};