#pragma once
#include "utils.h"
#include "Tile.hpp"
#include "Mathutils.hpp"
struct WorldChunk
{
	WorldChunk(Vector2i p_worldPos = Vector2i(0, 0), int p_worldID = 0);
	void cleanUp();
	Vector2i worldPos;
	int worldID;
	void setChunkTile(Vector2i p_chunkCoordinates);
	void fillRandom();

private:
	Tile** tiles = new Tile*[128];
};

