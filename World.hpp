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
#include "glm/glm.hpp"
#include <map>



struct WorldChunk;
namespace wc {
	struct ivec2 { // this is scuffed but I dont know how else to set the comparison operator for a thing I didnt write

		ivec2(int p_x, int p_y) {
			x = p_x;
			y = p_y;
		}
		ivec2(glm::ivec2 v2) {
			x = v2.x;
			y = v2.y;
		}

		int x;
		int y;
		friend bool operator<(const wc::ivec2& v1, const wc::ivec2& v2) {
			if (v1.y < v2.y) return true;
			if (v1.y > v2.y) return false;
			if (v1.x < v2.x) return true;
			else return false;
		}
	};
}

class World
{
public:
	World() { std::cout << (wc::ivec2(1, 0) < wc::ivec2(1, 1)) << std::endl; };
	~World();
	bool genChunk(glm::ivec2 p_chunkPos);
	bool genChunk(int p_chunkX, int p_chunkY);
	WorldChunk& getChunk(glm::ivec2 p_worldPos, bool& success);
	WorldChunk& getChunk(glm::ivec2 p_worldPos);
	void logSize();
	void logChunks();

private:
	std::map<wc::ivec2, WorldChunk> chunkMap;
	std::vector<Entity> worldEntities;
};

#endif WORLD_H