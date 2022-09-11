#pragma once
#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "Tile.hpp"
#include "Chunk.hpp"
#include <stdlib.h>
#include <iostream>
//#include "util/utils.hpp"
#include <util/ext/glm/glm.hpp>
#include "Camera.hpp"
#include "Framework/Window/GameWindow.hpp"
#include <GL/glew.h>
#include <map>
#include <queue>


class GameRenderer;
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
	World() {};
	bool genChunk(glm::ivec2 p_chunkPos);
	bool genChunk(int p_chunkX, int p_chunkY);
	void enqueueGen(glm::ivec2 p_chunkPos);
	void genFromQueue();
	bool autoGen(Camera& p_cam);

	WorldChunk& getChunk(glm::ivec2 p_worldPos, bool& success);
	bool chunkExistsAt(glm::ivec2 p_worldPos);
	bool removeChunk(glm::ivec2 p_worldPos);
	void removeChunk();
	void removeChunks();

	void drawWorld(GameRenderer& renderer, GameWindow& gw);
	void logSize();
	void logChunks();
	std::map<wc::ivec2, WorldChunk> chunkMap;
private:
	//std::vector<Entity> worldEntities;
	std::queue<glm::ivec2> loadQueue;


};

#endif WORLD_H