#include "Entity.hpp"

Entity::Entity(Vector2f p_pos, SDL_Texture* p_tex, int p_w, int p_h)
	:pos(p_pos), tex(p_tex)
{
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = p_w;
	srcRect.h = p_h;
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
SDL_Texture* Entity::getTex() {
	return tex;
}
SDL_Rect Entity::getSrcRect() {
	return srcRect;
}