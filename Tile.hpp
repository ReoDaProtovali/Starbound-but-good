#pragma once
#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include <SDL_image.h>
#include "MathUtils.hpp"
#include <vector>

class Tile
{
public:
	inline Tile() : chunkPos(Vector2i(0, 0)), tileID(0) {};
	inline Tile(Vector2i p_chunkPos, int p_tileID) : chunkPos(p_chunkPos), tileID(p_tileID) {};
	Vector2i getChunkPos();
	int tileID;
private:
	Vector2i chunkPos;
	//SDL_Texture* defaultTextures;
	//std::vector<SDL_Texture*> variationTextures;
};

#endif TILE_H

