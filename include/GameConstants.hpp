#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

#include <util/ext/glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

#define LOADLOGGING_ENABLED

constexpr auto CHUNKSIZE = 32;
enum class TextureID {
	NO_TEXTURE,
	STONE_TEXTURE,
	DIRT_TEXTURE,
	TILE_TEXTURE,
	REOHANG_TEXTURE,
	TESTSPRITESHEET_TEXTURE,
	ME_TEXTURE,
	CAMERA_FRAME_TEXTURE,
	REO_TEST,
	TILESHEET_TEXTURE
};

enum class SheetMode { // Sheet modes for CPU fetching. Better to use shaders in the future.
	BASIC,
	ANIMATION_ROWS
};

enum class Corner {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
};

#endif