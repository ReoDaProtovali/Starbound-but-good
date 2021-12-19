#pragma once

#ifndef CHUNK_H
#define CHUNK_H

#include "utils.hpp"
#include "SpriteSheet.hpp"
#include "Tile.hpp"
#include "Vertex.hpp"
#include "GL/glew.h"
#include "glm/glm.hpp"

#include <vector>
struct WorldChunk
{
	WorldChunk(glm::ivec2 p_worldPos = glm::ivec2(0, 0), int p_worldID = 0);
	Tile** getTiles();
	void cleanUp();
	glm::ivec2 worldPos;
	int worldID;
	const int chunkSize = 64;
	GLuint generateVBO(SpriteSheet p_spriteSheet);
	void setChunkTile(glm::ivec2 p_chunkCoordinates);
	void fillRandom();
	std::vector<Vertex> verts;


private:
	Tile** tiles = new Tile*[128];

};

#endif