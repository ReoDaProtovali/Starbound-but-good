#pragma once

#ifndef CHUNK_H
#define CHUNK_H

#include "utils.hpp"
#include "SpriteSheet.hpp"
#include "Tile.hpp"
#include "Vertex.hpp"
#include "GL/glew.h"
#include "glm/glm.hpp"
#include <noise/noise.h>

#include <vector>
struct WorldChunk
{
	WorldChunk(void) : worldID(-1),
		worldPos(glm::ivec2()),
		VAO(0),
		tiles(nullptr), invalid(true) {};
	WorldChunk(glm::ivec2 p_worldPos, int p_worldID);
	Tile** getTiles();
	void cleanUp();

	noise::module::Perlin noiseGenerator;
	void fillRandom();
	void worldGenerate(glm::ivec2 chunkPos);
	void setChunkTile(glm::ivec2 p_chunkCoordinates); // unimplemented

	GLuint generateVBO(SpriteSheet& p_spriteSheet);
	int getVBOSize();

	const int chunkSize = CHUNKSIZE;
	glm::ivec2 worldPos;
	int worldID;

	bool vboIsCurrent = false;
	GLuint VAO;

	bool invalid;
	bool isEmpty = true;

private:
	Tile** tiles;
	std::vector<Vertex> verts;

};

#endif