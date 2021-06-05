#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "MathUtils.hpp"
#include <vector>

class Tile
{
public:
	inline Tile(Vector2i p_chunkPos = Vector2i(-1,-1), int p_tileID = -1) : chunkPos(p_chunkPos), tileID(p_tileID) {};
	Vector2i getChunkPos();
	int tileID;
private:
	Vector2i chunkPos;
	//SDL_Texture* defaultTextures;
	//std::vector<SDL_Texture*> variationTextures;
};

