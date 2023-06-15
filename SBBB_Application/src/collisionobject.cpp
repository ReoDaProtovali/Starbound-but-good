#include "CollisionObject.hpp"
#include "GameConstants.hpp"

void CollisionObject::setColliderBodyDef(const b2BodyDef& p_def)
{
	m_def = p_def;
}

void CollisionObject::allowColliderRotation(bool p_enabled)
{
	// negated for intuitive sense
	m_def.fixedRotation = !p_enabled;
}

void CollisionObject::setColliderStartPosition(const glm::vec2& p_pos)
{
	m_def.position.Set(p_pos.x * TILES_TO_METERS, p_pos.y * TILES_TO_METERS);
}

void CollisionObject::setColliderAsStatic()
{
	m_def.type = b2_staticBody;
}

void CollisionObject::setColliderAsDynamic()
{
	m_def.type = b2_dynamicBody;
}

void CollisionObject::setColliderShapeAsBox(float p_w, float p_h)
{
	m_polygonShape.SetAsBox((p_w / 2.f) * TILES_TO_METERS, (p_h / 2.f) * TILES_TO_METERS);
	m_fixtureDef.shape = &m_polygonShape;
}

void CollisionObject::setColliderShapeAsCircle(float r)
{
	m_circleShape.m_p.SetZero();
	m_circleShape.m_radius = r * TILES_TO_METERS;
	m_fixtureDef.shape = &m_circleShape;
	isCircle = true;
}

void CollisionObject::setColliderFixtureDef(const b2FixtureDef& p_def)
{
	m_fixtureDef = p_def;
}

void CollisionObject::setFriction(float p_friction)
{
	m_fixtureDef.friction = p_friction;
}

void CollisionObject::setRestitution(float p_restitution)
{
	m_fixtureDef.restitution = p_restitution;
}

void CollisionObject::setRestitutionThreshold(float p_restitutionThreshold)
{
	m_fixtureDef.restitutionThreshold = p_restitutionThreshold;
}

void CollisionObject::setLinearDamping(float p_damping)
{
	m_body->SetLinearDamping(p_damping);
}

void CollisionObject::setDensity(float p_density)
{
	m_fixtureDef.density = p_density;
}

void CollisionObject::teleportColliderRelative(float xOff, float yOff)
{
	m_body->SetTransform(m_body->GetTransform().p + b2Vec2(xOff, yOff), m_body->GetAngle());
}

void CollisionObject::setVelocity(b2Vec2 p_velocity)
{
	m_body->SetLinearVelocity(p_velocity);
}

void CollisionObject::applyForce(b2Vec2 p_force)
{
	m_body->ApplyForceToCenter(p_force, true);
}

void CollisionObject::applyImpulse(b2Vec2 p_impulse)
{
	m_body->ApplyLinearImpulseToCenter(p_impulse, true);
}

void CollisionObject::applyForce(const glm::vec2& p_force)
{
	m_body->ApplyForceToCenter(b2Vec2(p_force.x, p_force.y), true);
}

void CollisionObject::applyImpulse(const glm::vec2& p_impulse)
{
	m_body->ApplyLinearImpulseToCenter(b2Vec2(p_impulse.x, p_impulse.y), true);
}

void CollisionObject::wake()
{
	m_body->SetAwake(true);
}

void CollisionObject::updateValues()
{
	bodyVelocity = glm::vec2(m_body->GetLinearVelocity().x, m_body->GetLinearVelocity().y);
}

void CollisionObject::spawnCollider(b2World& p_world)
{
	m_body = p_world.CreateBody(&m_def);
	if (isCircle) {
		m_fixtureDef.shape = &m_circleShape;
	}
	else {
		m_fixtureDef.shape = &m_polygonShape;
	}
	m_body->CreateFixture(&m_fixtureDef);
}

void CollisionObject::respawnCollider(b2World& p_world)
{
	p_world.DestroyBody(m_body);
	m_body = p_world.CreateBody(&m_def);
	if (isCircle) {
		m_fixtureDef.shape = &m_circleShape;
	}
	else {
		m_fixtureDef.shape = &m_polygonShape;
	}
	m_body->CreateFixture(&m_fixtureDef);

}

b2Transform CollisionObject::getCollisionTransform()
{
	auto transform = m_body->GetTransform();
	transform.p.x *= METERS_TO_TILES;
	transform.p.y *= METERS_TO_TILES;

	return transform;
}

b2Transform CollisionObject::getCollisionTransformMeters()
{
	return m_body->GetTransform();
}

glm::vec2 CollisionObject::getVelocity()
{
	return bodyVelocity.load();
}

RayCastOutput CollisionObject::rayCastRelative(float offX, float offY, float lenX, float lenY)
{
	b2Transform transform = m_body->GetTransform();

	float pX = transform.p.x + offX * TILES_TO_METERS;
	float pY = transform.p.y + offY * TILES_TO_METERS;
	b2Vec2 p1 = b2Vec2(pX, pY);
	b2Vec2 p2 = b2Vec2(pX + lenX * TILES_TO_METERS, pY + lenY * TILES_TO_METERS);

	RayCastCallback cb;
	m_body->GetWorld()->RayCast(&cb, p1, p2);
	cb.out.point -= glm::vec2(transform.p.x + offX * TILES_TO_METERS, transform.p.y + offY * TILES_TO_METERS); // make it relative
	cb.out.point *= METERS_TO_TILES;
	if (!cb.out.hit) cb.out.point = glm::vec2(lenX, lenY);
	cb.out.rayStart = glm::vec2(p1.x, p1.y) * METERS_TO_TILES;
	return cb.out;
}
