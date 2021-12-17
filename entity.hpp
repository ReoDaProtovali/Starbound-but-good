#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include <SDL.h>
#include <vector>
#include "MathUtils.hpp"

class Entity
{
public:
	Entity(Vector2f p_pos, int p_w = 16, int p_h = 16);
	Vector2f getPos();
	void setPos(Vector2f& p_pos);
	void setX(float p_x);
	void setY(float p_y);
private:
	Vector2f pos;
};

#endif ENTITY_H