#pragma once
#include <vector>
#include "Tile.hpp"
#include "Chunk.h"
#include "Entity.hpp"
#include <stdlib.h>
#include "utils.h"
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

