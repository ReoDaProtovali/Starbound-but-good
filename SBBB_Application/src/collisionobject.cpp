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
	m_shape.SetAsBox((p_w / 2.f) * TILES_TO_METERS, (p_h / 2.f) * TILES_TO_METERS);
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

void CollisionObject::setDensity(float p_density)
{
	m_fixtureDef.density = p_density;
}

void CollisionObject::spawnCollider(b2World& p_world)
{
	m_body = p_world.CreateBody(&m_def);
	m_fixtureDef.shape = &m_shape;
	m_body->CreateFixture(&m_fixtureDef);
}

void CollisionObject::respawnCollider(b2World& p_world)
{
	p_world.DestroyBody(m_body);
	m_body = p_world.CreateBody(&m_def);
	m_fixtureDef.shape = &m_shape;
	m_body->CreateFixture(&m_fixtureDef);

}

b2Transform CollisionObject::getCollisionTransform()
{
	auto transform = m_body->GetTransform();
	transform.p.x *= METERS_TO_TILES;
	transform.p.y *= METERS_TO_TILES;

	return transform;
}
