#include "Entity.hpp"

Entity::Entity(glm::vec2 p_pos, int p_w, int p_h)
	: pos(p_pos)
{
}
glm::vec2 Entity::getPos() {
	return pos;
}
void Entity::setX(float p_x) {
	pos.x = p_x;
}
void Entity::setY(float p_y) {
	pos.y = p_y;
}
void Entity::setPos(glm::vec2& p_pos) {
	pos = p_pos;
}
