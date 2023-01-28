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

	Tile() : m_chunkPos(glm::ivec2(0, 0)), m_tileID(0) {};
	Tile(int p_x, int p_y, uint32_t p_tileID) : m_chunkPos(p_x, p_y), m_tileID(p_tileID) {};
	Tile(glm::ivec2 p_chunkPos, uint32_t p_tileID) : m_chunkPos(p_chunkPos), m_tileID(p_tileID) {};
	glm::ivec2 getChunkPos();
private:
	glm::ivec2 m_chunkPos;
};

#endif TILE_H