#include "Player.hpp"

void Player::onSpawn(b2World& p_world) {
	setColliderStartPosition(glm::vec2(m_position.x, m_position.y));

	setColliderAsDynamic();
	setDensity(1.f);
	allowColliderRotation(false);

	setColliderShapeAsBox(m_entityDims.x / 2.f, m_entityDims.y - 0.02f);
	//setColliderShapeAsCircle(1.5f);
	setFriction(1.0f);

	setRestitution(0.00f);

	m_def.awake = true;
	m_def.allowSleep = false;

	spawnCollider(p_world);
}
void Player::onUpdate()
{
	m_posMut.lock();
	CollisionObject::updateValues();
	m_AI.update();
	auto transform = getCollisionTransform();
	m_position.x = transform.p.x;
	m_position.y = transform.p.y;
	//setRotation(transform.q.GetAngle());
	float interp_factor;
	if (Globals::shouldInterpolate())
		interp_factor = 1.f;
	else
		interp_factor = 0.4f;
	const float anticipationScale = 2.f * METERS_TO_TILES * (1.f / UPDATE_RATE_FPS);
	const float anticipationSpeed = interp_factor;
	glm::vec3 newPos = utils::lerp(entityCam.pos, glm::vec3(m_position.x + getVelocity().x * anticipationScale, m_position.y + getVelocity().y * anticipationScale, entityCam.pos.z), anticipationSpeed);
	entityCam.pos = newPos;

	//RayCastOutput rc = rayCastRelative(0.f, 0.f, 0.f, -20.f);
	//if (rc.hit)
	//	applyForce(glm::vec2(0.f, (20.f + rc.point.y)) * 4.f);

	if (m_body->GetLinearVelocity().x < 0 && fabs(getVelocity().x) > 0.1f) {
		setScale(glm::vec2(-1.f, 1.f));
	}
	if (m_body->GetLinearVelocity().x > 0 && fabs(getVelocity().x) > 0.1f) {
		setScale(glm::vec2(1.f, 1.f));
	}
	updateTimestamp();
	m_posMut.unlock();
}

void Player::draw(DrawSurface& p_target, DrawStates& p_states)
{
	m_posMut.lock_shared();
	if (m_sprite.bounds.xy.x == 0) { // scuffed init checking
		m_sprite.setBounds(Rect(0.f, 0.f, m_entityDims.x + 0.03f, m_entityDims.y + 0.03f));
		m_sprite.attachTexture(ResourceManager::Get().getTexture("reotest"));
	}

	m_sprite.cloneTransform(*this);

	m_sprite.setOriginRelative(OriginLoc::CENTER);
	m_sprite.draw(p_target, p_states);

	m_posMut.unlock_shared();
}

void Player::drawRayCast(RayCastOutput& out, DrawSurface& p_target, DrawStates& p_states)
{
	glm::vec3 col = glm::vec3(0.f, 1.f, 0.f);
	if (!out.hit) col = glm::vec3(1.f, 0.f, 0.f);
	SBBBDebugDraw::drawLineImmediate(
		out.rayStart.x,
		out.rayStart.y,
		out.rayStart.x + out.point.x,
		out.rayStart.y + out.point.y,
		col,
		p_target, p_states);
}
