#pragma once

#ifndef CHUNK_H
#define CHUNK_H

#include "utils.h"
#include "SpriteSheet.h"
#include "Tile.hpp"
#include "MathUtils.hpp"


#include <vector>
struct WorldChunk
{
	WorldChunk(Vector2i p_worldPos = Vector2i(0, 0), int p_worldID = 0);
	Tile** getTiles();
	void cleanUp();
	Vector2i worldPos;
	int worldID;
	void generateVBO(SpriteSheet p_spriteSheet, unsigned int p_vao);
	void setChunkTile(Vector2i p_chunkCoordinates);
	void fillRandom();
	std::vector<float> verts;


private:
	Tile** tiles = new Tile*[128];

};

#endif