#pragma once
#include <box2d.h>
#include "util/SharedDynArray.hpp"
#include "TestEntity.hpp"
// Primary game logic, entities, collision and stuff
class Simulation {
public:
	Simulation() {};

	void init();

	void tick();

private:
	b2World m_physicsWorld{b2Vec2(0.f, -10.f)};
	SharedDynArray<TestEntity>& m_entities = SharedDynArray<TestEntity>::Get();
};