#pragma once
#include <util/ext/glm/glm.hpp>
#include "util/Bitwise.hpp"
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
		set6BitX(p_x);
		set6BitY(p_y);
		set5BitZ(p_z);
		set15BitID(p_id);
	}
	Bitwise32 xyzID; // 32 bit unsigned integer storing multiple components
	uint32_t adjacent = 0;
	uint32_t variationCount = 0;

	void set6BitX(uint32_t p_x) {
		xyzID.setRange(26, 31, p_x);
	}
	void set6BitY(uint32_t p_y) {
		xyzID.setRange(20, 25, p_y);
	}
	void set5BitZ(uint32_t p_z) {
		xyzID.setRange(15, 19, p_z);
	}
	void set15BitID(uint32_t p_id) {
		xyzID.setRange(0, 14, p_id);
	}
	void setAdjacent(Adjacency a) {
		adjacent = a | adjacent;
	}
	void setAdjacent(uint32_t a) {
		adjacent = a | adjacent;
	}
	glm::uvec4 toUvec4() { // undoes the packing and formats nicely
		return glm::uvec4(
			xyzID.unpackRange(26, 31), 
			xyzID.unpackRange(20, 25), 
			xyzID.unpackRange(15, 19), 
			xyzID.unpackRange(0, 14)
		);
	}
};