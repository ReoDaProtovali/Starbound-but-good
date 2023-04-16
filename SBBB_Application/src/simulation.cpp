#include "Simulation.hpp"
#include "GameConstants.hpp"
#include <cstdlib>
void Simulation::init()
{
	for (int i = 0; i < 500; i++) {
		m_entities.emplace(-16.f + (float(rand() % 10) - 5.f), 515.f + (float(rand() % 300) - 5.f), 2.f, 2.f, false);
	}
	m_entities.emplace(-16.f, 96.f, 200.f, 5.f, true);
	m_entities.emplace(-16.f - 100.f, 106.f, 5.f, 20.f, true);
	m_entities.emplace(-16.f + 100.f, 106.f, 5.f, 20.f, true);


	for (auto& entity : m_entities) {
		entity.onSpawn(m_physicsWorld);
	}

}

void Simulation::tick()
{

	m_physicsWorld.Step(1.f / (float)UPDATE_RATE_FPS, 6, 3);
	//if (rand() % 20 > 18) { 
	//	size_t randomIndex = rand() % m_entities.length();
	//	auto entityOpt = m_entities.at(randomIndex);
	//	if (entityOpt.has_value()) {
	//		entityOpt.value().get().onDestroy(m_physicsWorld);
	//	}
	//	m_entities.invalidate(randomIndex);
	//};
	for (auto& entity : m_entities) {
		entity.onUpdate();
	}
}
