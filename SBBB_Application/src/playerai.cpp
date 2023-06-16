#include "PlayerAI.hpp"

void PlayerAI::update()
{
	assert(m_host);
	moveSpeed = 70.f;
	auto vel = m_host->getVelocity();
	float vertdistAhead = std::max(std::abs(vel.y) / 15.f, 0.3f);

	auto raycast1 = m_host->rayCastRelative(-0.745f, -1.4f, 0.f, -vertdistAhead);
	auto raycast2 = m_host->rayCastRelative(0.f, -1.4f, 0.f, -vertdistAhead);
	auto raycast3 = m_host->rayCastRelative(0.745f, -1.4f, 0.f, -vertdistAhead);
	bool touchingGround = raycast1.hit || raycast2.hit || raycast3.hit;

	bool jump = false;
	while (auto e = m_keyObserver.observe()) {
		KeyEvent event = e.value();
		// we don't actually want to use the controller for up, as that would be a lil weird for a player to just start rising
		switch (event.keyCode) {
		case SDLK_w:
			if (event.wasDown && touchingGround) {
				m_host->applyImpulse(b2Vec2(0.f, jumpStrength));
				jump = true;
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

	const float groundSpeedLimit = 10.f;
	const float airSpeedLimit = 20.f;
	if (controller.getMovementDir().x > 0 && vel.x < airSpeedLimit)
		m_host->applyForce(controller.getMovementDir() * moveSpeed * (touchingGround ? 1.f : 0.2f));
	else if (controller.getMovementDir().x < 0 && vel.x > -airSpeedLimit)
		m_host->applyForce(controller.getMovementDir() * moveSpeed * (touchingGround ? 1.f : 0.2f));

	if (!touchingGround && controller.getMovementDir() != glm::vec2(0.f)) {
		if (std::abs(vel.x) > groundSpeedLimit) {
			float adjustment = (groundSpeedLimit - std::abs(vel.x)) * 0.2f;
			m_host->setVelocity(b2Vec2(vel.x + (adjustment * (vel.x > 0 ? 1.f : -1.f)), vel.y));
		}
	}
	if (jump) return;
	vel = m_host->getVelocity();

	if (controller.getMovementDir() == glm::vec2(0.f) && touchingGround) {
		m_host->setVelocity(b2Vec2(vel.x * 0.7f, vel.y));
	};

	if (std::abs(vel.x) > groundSpeedLimit && touchingGround && controller.getMovementDir() != glm::vec2(0.f)) {
		float adjustment = (groundSpeedLimit - std::abs(vel.x)) * 0.4f;
		m_host->setVelocity(b2Vec2(vel.x + (adjustment * (vel.x > 0? 1.f : -1.f)), vel.y));
	}

	if (std::abs(vel.y) > 10.f) return;

	// raycast hell
	float distAhead = std::max(std::abs(vel.x) / 20.f, 0.15f);
	auto raycastlb = m_host->rayCastRelative(-0.7f, -1.5f, -distAhead, 0.0f).hit;
	auto raycastlm = m_host->rayCastRelative(-0.7f, 0.0f, -distAhead, 0.0f).hit;
	auto raycastlt = m_host->rayCastRelative(-0.7f, 1.0f, -distAhead, 0.0f).hit;
	auto raycastlh = m_host->rayCastRelative(-0.7f, 2.0f, -distAhead, 0.0f).hit;
	auto raycastlhh = m_host->rayCastRelative(-0.7f, 3.0f, -distAhead, 0.0f).hit;

	auto raycastrb = m_host->rayCastRelative(0.7f, -1.5f, distAhead, 0.0f).hit;
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
		m_host->setVelocity(b2Vec2(vel.x * 0.8f, vel.y));
	}
	else if (raycastrb && !(raycastrm || raycastrt || raycastrh || raycastrhh || raycasttr || raycasttm)) {
		float tileY = m_host->getCollisionTransform().p.y - 1.49f;
		float newY = std::ceil(tileY) - tileY + 0.1f;
		m_host->teleportColliderRelative(0.05f * 0.8f, newY * TILES_TO_METERS);
		m_host->setVelocity(b2Vec2(vel.x, vel.y));
	}

}
