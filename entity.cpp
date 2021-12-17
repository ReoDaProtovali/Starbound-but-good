#include "Entity.hpp"

Entity::Entity(Vector2f p_pos, int p_w, int p_h)
	: pos(p_pos)
{
}
Vector2f Entity::getPos() {
	return pos;
}
void Entity::setX(float p_x) {
	pos.x = p_x;
}
void Entity::setY(float p_y) {
	pos.y = p_y;
}
void Entity::setPos(Vector2f& p_pos) {
	pos = p_pos;
}
