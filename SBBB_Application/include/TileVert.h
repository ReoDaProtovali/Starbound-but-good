#pragma once
#include <util/ext/glm/glm.hpp>

enum Adjacency {
	TL = 0b10000000,
	T =  0b01000000,
	TR = 0b00100000,
	L =  0b00010000,
	R =  0b00001000,
	BL = 0b00000100,
	B =  0b00000010,
	BR = 0b00000001
};
struct TileVert { // efficient mesh building, xy are 6 bit, z is 5 bit, and ID is 15 bit. (insanity)
	TileVert() {};
	TileVert(uint32_t p_x, uint32_t p_y, uint32_t p_z, uint32_t p_id) { /// compact version, packs the values upon construction
		xyzID |= (0xFC000000 & (p_x << 26)) | (0x3F00000 & (p_y << 20)) | (0xF8000 & (p_z << 15)) | (0x7FFF & (p_id));
	}
	uint32_t xyzID = 0; // 32 bit unsigned integer storing multiple components
	uint32_t adjacent = 0;
	uint32_t variationCount = 0;

	void set5BitX(uint32_t p_x) {
		uint32_t shifted = p_x << 26; // Bit shift the input 26 places such that the first 6 bits are aligned
		uint32_t bitmask = 0b11111100000000000000000000000000; // Only modify selected bits
		xyzID |= bitmask & shifted; // Delete extraneous data and set the bits correctly
	}
	void set5BitY(uint32_t p_y) {
		uint32_t shifted = p_y << 20; // Bit shift the input 20 places such that the first 6 bits are aligned
		uint32_t bitmask = 0b00000011111100000000000000000000;
		xyzID |= bitmask & shifted;
	}
	void set5BitZ(uint32_t p_z) {
		uint32_t shifted = p_z << 15; // Bit shift the input 15 places such that the first 5 bits are aligned
		uint32_t bitmask = 0b00000000000011111000000000000000;
		xyzID |= bitmask & shifted;
	}
	void set16BitID(uint32_t p_id) {
		uint32_t shifted = p_id; // Input is where it should be, just needs trimmed
		uint32_t bitmask = 0b00000000000000000111111111111111;
		xyzID |= bitmask & shifted;
	}
	void setAdjacent(Adjacency a) {
		adjacent = a | adjacent;
	}
	glm::uvec4 toUvec4() { // undoes the packing and formats nicely
		uint32_t x = (0xFC000000 & xyzID) >> 26;
		uint32_t y = (0x3F00000 & xyzID) >> 20;
		uint32_t z = (0xF8000 & xyzID) >> 15;
		uint32_t ID = (0x7FFF & xyzID);
		return glm::uvec4(x, y, z, ID);
	}
};