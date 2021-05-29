#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Mathutils.hpp"
#include <vector>

class Tile
{
public:
	inline Tile(Vector2i p_chunkPos = Vector2i(-1,-1), int p_tileID = -1) : chunkPos(p_chunkPos), tileID(p_tileID) {};
	Vector2i getChunkPos();

private:
	Vector2i chunkPos;
	int tileID;
	//SDL_Texture* defaultTextures;
	//std::vector<SDL_Texture*> variationTextures;
};

