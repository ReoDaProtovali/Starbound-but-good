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
#ifdef SBBB_DEBUG
	for (int i = 0; i < 0; i++) {
		m_entities.emplace_front(new TestEntity(float(rand() % 20), float(rand() % 20) + 100.f, 1.f, 1.f, false));
		//(*m_entities.begin()).m_eptr->wake();
	}
#else
	for (int i = 0; i < 0; i++) {
		m_entities.emplace_front(new TestEntity((rand() % 20), (rand() % 20) + 100.f, 1.f, 1.f, false));
	}
#endif
	m_entities.emplace_front(new Player(0.f, 100.f, 3.f, 3.f));

	player = (*m_entities.begin()).m_eptr;
}

void Simulation::tick()
{
	static Observer<KeyEvent> keyObserver{ globals.keySubject };
	while (auto opt = keyObserver.observe()) {
		switch (opt.value().keyCode) {
		case SDLK_b:
			if (!player) break;
			b2Vec2 explosionCenter = b2Vec2(player->getPosition().x * TILES_TO_METERS, player->getPosition().y * TILES_TO_METERS); // x and y are the coordinates of the explosion center
			float explosionRadius = 10.0f; // the radius of the explosion
			float explosionMagnitude = 100.0f; // the magnitude of the explosion
			for (b2Body* body = m_physicsWorld.GetBodyList(); body != nullptr; body = body->GetNext())
			{
				b2Vec2 bodyCenter = body->GetWorldCenter();
				float distance = (explosionCenter - bodyCenter).Length();

				if (distance < explosionRadius)
				{
					b2Vec2 direction = (bodyCenter - explosionCenter);
					direction.Normalize();
					float magnitude = explosionMagnitude * (1 - distance / explosionRadius);
					body->ApplyLinearImpulseToCenter(magnitude * direction, true);
				}
			}
			break;
		}
	}

	if (globals.worldDoneGenerating) {
		m_physicsWorld.SetGravity(b2Vec2(0.f, -29.f));
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
