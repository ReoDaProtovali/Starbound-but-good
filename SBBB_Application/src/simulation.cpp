#include "Simulation.hpp"
#include "GameConstants.hpp"
#include <cstdlib>
#include "util/SubjectObserver.hpp"
#include "Player.hpp"
void Simulation::init()
{

	//m_entities.emplace(-16.f, 96.f, 200.f, 5.f, true);
	//m_entities.emplace(-16.f - 100.f, 106.f, 5.f, 20.f, true);
	//m_entities.emplace(-16.f + 100.f, 106.f, 5.f, 20.f, true);

	spawnTestEntities();

	for (auto& entity : m_entities) {
		entity->onSpawn(m_physicsWorld);
	}

}

void Simulation::spawnTestEntities()
{
	for (int i = 0; i < 200; i++) {
		m_entities.emplace_front(new TestEntity((rand() % 20), (rand() % 20) + 100.f, 1.f, 1.f, false));
	}
	m_entities.emplace_front(new Player(0.f, 100.f, 3.f, 3.f));
}

void Simulation::tick()
{
	//static Observer<KeyEvent> keyObserver;
	//while (auto opt = keyObserver.observe()) {
	//	switch (opt.value().keyCode) {
	//	case SDLK_g:
	//		if (opt.value().wasDown) {
	//			m_physicsWorld.SetGravity(b2Vec2(0, -10.f));
	//		}
	//		else {
	//			m_physicsWorld.SetGravity(b2Vec2(0, 0.f));
	//		}
	//		break;
	//	case SDLK_t:
	//		if (opt.value().wasDown) {
	//			m_physicsWorld.SetGravity(b2Vec2(0, 10.f));
	//		}
	//		else {
	//			m_physicsWorld.SetGravity(b2Vec2(0, 0.f));
	//		}
	//		break;
	//	case SDLK_f:
	//		if (opt.value().wasDown) {
	//			m_physicsWorld.SetGravity(b2Vec2(-10.f, 0.f));
	//		}
	//		else {
	//			m_physicsWorld.SetGravity(b2Vec2(0, 0.f));
	//		}
	//		break;
	//	case SDLK_h:
	//		if (opt.value().wasDown) {
	//			m_physicsWorld.SetGravity(b2Vec2(10.f, 0.f));
	//		}
	//		else {
	//			m_physicsWorld.SetGravity(b2Vec2(0, 0.f));
	//		}
	//		break;
	//	}

	//}

	if (globals.worldDoneGenerating) {
		m_physicsWorld.SetGravity(b2Vec2(0.f, -20.f));
	}
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
		entity->onUpdate();
	}
}
