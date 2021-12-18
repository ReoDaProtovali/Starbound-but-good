#pragma once
#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "Tile.hpp"
#include "Chunk.hpp"
#include "Entity.hpp"
#include <stdlib.h>
#include <iostream>
#include "utils.hpp"
class WorldChunk;

class World
{
public:
	World();
	~World();
	WorldChunk& getChunk(Vector2i p_worldPos);

private:
	std::vector<WorldChunk> chunks;
	std::vector<Entity> worldEntities;
};

#endif WORLD_H