#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "MathUtils.hpp"

class Entity
{
public:
	Entity(Vector2f p_pos, SDL_Texture* p_tex, int p_w = 16, int p_h = 16);
	Vector2f getPos();
	void setPos(Vector2f& p_pos);
	void setX(float p_x);
	void setY(float p_y);
	SDL_Texture* getTex();
	SDL_Rect getSrcRect();
private:
	Vector2f pos;
	SDL_Rect srcRect;
	SDL_Texture* tex;
};

