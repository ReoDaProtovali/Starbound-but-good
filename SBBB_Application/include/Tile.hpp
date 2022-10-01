#pragma once
#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include <util/ext/glm/vec2.hpp>
#include <vector>

class Tile
{
public:
	Tile() : chunkPos(glm::ivec2(0, 0)), tileID(0) {};
	Tile(glm::ivec2 p_chunkPos, unsigned int p_tileID) : chunkPos(p_chunkPos), tileID(p_tileID) {};
	glm::ivec2 getChunkPos();
	unsigned int tileID;
private:
	glm::ivec2 chunkPos;
};

#endif TILE_H

