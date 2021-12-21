#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include <SDL.h>
#include <vector>
#include "glm/vec2.hpp"


class Entity
{
public:
	Entity(glm::vec2 p_pos, int p_w = 16, int p_h = 16);
	glm::vec2 getPos();
	void setPos(glm::vec2& p_pos);
	void setX(float p_x);
	void setY(float p_y);
private:
	glm::vec2 pos;
};

#endif ENTITY_H