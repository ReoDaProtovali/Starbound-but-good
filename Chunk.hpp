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

struct TileVert { // efficient mesh building, xy are 6 bit, z is 5 bit, and ID is 15 bit. (insanity)
	TileVert() {};
	TileVert(Uint32 p_x, Uint32 p_y, Uint32 p_z, Uint32 p_id) { /// compact version, packs the values upon construction
		xyzID |= (0xFC000000 & (p_x << 26)) | (0x3F00000 & (p_y << 20)) | (0xF8000 & (p_z << 15)) | (0x7FFF & (p_id));
	}
	Uint32 xyzID = 0; // 32 bit unsigned integer storing multiple components

	void set5BitX(Uint32 p_x) {
		Uint32 shifted = p_x << 26; // Bit shift the input 26 places such that the first 6 bits are aligned
		Uint32 bitmask = 0b11111100000000000000000000000000; // Only modify selected bits
		xyzID |= bitmask & shifted; // Delete extraneous data and set the bits correctly
	}
	void set5BitY(Uint32 p_y) {
		Uint32 shifted = p_y << 20; // Bit shift the input 20 places such that the first 6 bits are aligned
		Uint32 bitmask = 0b00000011111100000000000000000000;
		xyzID |= bitmask & shifted;
	}
	void set5BitZ(Uint32 p_z) {
		Uint32 shifted = p_z << 15; // Bit shift the input 15 places such that the first 5 bits are aligned
		Uint32 bitmask = 0b00000000000011111000000000000000;
		xyzID |= bitmask & shifted;
	}
	void set16BitID(Uint32 p_id) {
		Uint32 shifted = p_id; // Input is where it should be, just needs trimmed
		GLuint bitmask = 0b00000000000000000111111111111111;
		xyzID |= bitmask & shifted;
	}

	glm::uvec4 toUvec4() { // undoes the packing and formats nicely
		Uint32 x = (0xFC000000 & xyzID) >> 26;
		Uint32 y = (0x3F00000 & xyzID) >> 20;
		Uint32 z = (0xF8000 & xyzID) >> 15;
		Uint32 ID = (0x7FFF & xyzID);
		return glm::uvec4(x, y, z, ID);
	}
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
	void generateVBO();
	unsigned int getVBOSize();

	void remove();

	const int chunkSize = CHUNKSIZE;
	glm::ivec2 worldPos;
	int worldID;

	bool meshIsCurrent = false;
	bool invalid = false;
	bool isEmpty = true;
	Mesh<TileVert> tileMesh;

private:
	Array2D<Tile> tiles;

};

#endif