#pragma once
#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

#include <glm.hpp>
#include <vector>
#include "Vertex.hpp"

typedef enum class TextureID {
	NO_TEXTURE,
	STONE_TEXTURE,
	DIRT_TEXTURE,
	TILE_TEXTURE,
	REOHANG_TEXTURE,
	TESTSPRITESHEET_TEXTURE,
	ME_TEXTURE,
	TILESHEET_TEXTURE
} TextureID;

enum class SheetMode {
	TILESHEET,
	ANIMATION_ROWS
};

enum class Corner {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
};

typedef enum t_attrib_id
{
	attrib_position,
	attrib_texCoord
} t_attrib_id;


#endif