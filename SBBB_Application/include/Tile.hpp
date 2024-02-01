#pragma once
#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include <util/ext/glm/vec2.hpp>
#include <util/ext/glm/vec3.hpp>
#include <vector>

class Tile
{
public:
	uint32_t m_tileID;

	Tile() : m_tileID(0) {};
	Tile(uint32_t p_tileID) : m_tileID(p_tileID) {};
};

#endif TILE_H