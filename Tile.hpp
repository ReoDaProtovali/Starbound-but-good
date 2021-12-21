#pragma once
#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include "glm/vec2.hpp"
#include <vector>

class Tile
{
public:
	inline Tile() : chunkPos(glm::ivec2(0, 0)), tileID(0) {};
	inline Tile(glm::ivec2 p_chunkPos, int p_tileID) : chunkPos(p_chunkPos), tileID(p_tileID) {};
	glm::ivec2 getChunkPos();
	int tileID;
private:
	glm::ivec2 chunkPos;
};

#endif TILE_H

