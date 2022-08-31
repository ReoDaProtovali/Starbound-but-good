#pragma once

#ifndef CHUNK_H
#define CHUNK_H

#include "utils.hpp"
#include "SpriteSheet.hpp"
#include "Tile.hpp"
#include "Mesh.hpp"
#include "GL/glew.h"
#include "glm/glm.hpp"
#include <noise/noise.h>

#include <vector>
#include "Array2D.hpp"

struct TestVert { // temporary schema
	TestVert(GLfloat p_x, GLfloat p_y, GLfloat p_z, GLfloat p_u, GLfloat p_v) :
		x(p_x), y(p_y), z(p_z), u(p_u), v(p_v) {};
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat u;
	GLfloat v;
};
struct TileVert {
	TileVert(GLfloat p_x, GLfloat p_y, GLfloat p_z, GLuint p_id) :
		x(p_x), y(p_y), z(p_z), id(p_id) {};
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLuint id;
};
struct WorldChunk
{
	WorldChunk(void) : worldID(-1),
		worldPos(glm::ivec2()),
		tiles(Array2D<Tile>()), 
		invalid(true) {};
	WorldChunk(glm::ivec2 p_worldPos, int p_worldID);

	noise::module::Perlin noiseGenerator;
	void fillRandom();
	void worldGenerate(glm::ivec2 p_chunkPos);
	void setChunkTile(glm::ivec2 p_chunkCoordinates); // unimplemented

	Tile* getTiles();
	void generateVBO(SpriteSheet& p_spriteSheet);
	unsigned int getVBOSize();

	void remove();

	const int chunkSize = CHUNKSIZE;
	glm::ivec2 worldPos;
	int worldID;

	bool meshIsCurrent = false;
	bool invalid = false;
	bool isEmpty = true;
	Mesh<TestVert> tileMesh;

private:
	Array2D<Tile> tiles;

};

#endif