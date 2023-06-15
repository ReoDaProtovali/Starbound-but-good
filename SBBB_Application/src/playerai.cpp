#include "PlayerAI.hpp"

void PlayerAI::update()
{
	assert(m_host);
	auto vel = m_host->getVelocity();
	while (auto e = m_keyObserver.observe()) {
		KeyEvent event = e.value();
		// we don't actually want to use the controller for up, as that would be a lil weird for a player to just start rising
		switch (event.keyCode) {
		case SDLK_w:
			if (event.wasDown) {
				float vertdistAhead = std::max(std::abs(vel.y) / 15.f, 0.15f);

				auto raycast1 = m_host->rayCastRelative(-0.7f, -1.4f, 0.f, -vertdistAhead);
				auto raycast2 = m_host->rayCastRelative(0.f, -1.4f, 0.f, -vertdistAhead);
				auto raycast3 = m_host->rayCastRelative(0.7f, -1.4f, 0.f, -vertdistAhead);
				if (raycast1.hit || raycast2.hit || raycast3.hit)
					m_host->applyImpulse(b2Vec2(0.f, jumpStrength));
			}
			break;
		case SDLK_a:
			controller.dpadControl(CardinalDirection::LEFT, event.wasDown);
			break;
		case SDLK_s: // fall faster, why not
			controller.dpadControl(CardinalDirection::DOWN, event.wasDown);
			break;
		case SDLK_d:
			controller.dpadControl(CardinalDirection::RIGHT, event.wasDown);
			break;
		}
	}
	//if (controller.getMovementDir() == glm::vec2(0.f)) return;
	m_host->applyForce(controller.getMovementDir() * moveSpeed);

	if (std::abs(vel.y) > 10.f) return;

	// raycast hell
	float distAhead = std::max(std::abs(vel.x) / 20.f, 0.15f);
	auto raycastlb = m_host->rayCastRelative(-0.7f, -1.4f, -distAhead, 0.0f).hit;
	auto raycastlm = m_host->rayCastRelative(-0.7f, 0.0f, -distAhead, 0.0f).hit;
	auto raycastlt = m_host->rayCastRelative(-0.7f, 1.0f, -distAhead, 0.0f).hit;
	auto raycastlh = m_host->rayCastRelative(-0.7f, 2.0f, -distAhead, 0.0f).hit;
	auto raycastlhh = m_host->rayCastRelative(-0.7f, 3.0f, -distAhead, 0.0f).hit;

	auto raycastrb = m_host->rayCastRelative(0.7f, -1.4f, distAhead, 0.0f).hit;
	auto raycastrm = m_host->rayCastRelative(0.7f, 0.0f, distAhead, 0.0f).hit;
	auto raycastrt = m_host->rayCastRelative(0.7f, 1.0f, distAhead, 0.0f).hit;
	auto raycastrh = m_host->rayCastRelative(0.7f, 2.0f, distAhead, 0.0f).hit;
	auto raycastrhh = m_host->rayCastRelative(0.7f, 3.0f, distAhead, 0.0f).hit;

	auto raycasttl = m_host->rayCastRelative(-0.7f, 1.45f, 0.f, 0.15f).hit;
	auto raycasttm = m_host->rayCastRelative(0.0f, 1.45f, 0.f, 0.15f).hit;
	auto raycasttr = m_host->rayCastRelative(0.7f, 1.45f, 0.f, 0.15f).hit;

	if (raycastlb && !(raycastlm || raycastlt || raycastlh || raycastlhh || raycasttl || raycasttm)) {
		float tileY = m_host->getCollisionTransform().p.y - 1.49f;
		float newY = std::ceil(tileY) - tileY + 0.1f;
		m_host->teleportColliderRelative(-0.05f, newY * TILES_TO_METERS);
		m_host->setVelocity(b2Vec2(vel.x, vel.y));
	}
	else if (raycastrb && !(raycastrm || raycastrt || raycastrh || raycastrhh || raycasttr || raycasttm)) {
		float tileY = m_host->getCollisionTransform().p.y - 1.49f;
		float newY = std::ceil(tileY) - tileY + 0.1f;
		m_host->teleportColliderRelative(0.05f, newY * TILES_TO_METERS);
		m_host->setVelocity(b2Vec2(vel.x, vel.y));
	}
	//}


}
