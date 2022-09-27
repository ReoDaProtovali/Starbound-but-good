#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

#include "Framework/FrameworkConstants.hpp"
#define GAME_UPDATE_SPEED 165
#define FRAMES_BETWEEN_STAT_UPDATES 120
#define DISABLE_RUNTIME_CONSOLE false
#define LOADLOGGING_ENABLED
#define DELETELOGGING_ENABLED

// do NOT increase this. it does NOT like to be changed easily
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

enum DefaultTileID {
	EMPTY,
	STONE,
	DIRT
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