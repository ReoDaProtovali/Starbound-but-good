#include "Entity.hpp"

Entity::Entity(float p_x, float p_y, SDL_Texture* p_tex, float p_w, float p_h)
	:x(p_x), y(p_y), tex(p_tex)
{
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = p_w;
	srcRect.h = p_h;
}
float Entity::getX() {
	return x;
}
float Entity::getY() {
	return y;
}
void Entity::setX(float p_x) {
	x = p_x;
}
void Entity::setY(float p_y) {
	y = p_y;
}
SDL_Texture* Entity::getTex() {
	return tex;
}
SDL_Rect Entity::getSrcRect() {
	return srcRect;
}