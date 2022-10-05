#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

#include "Framework/FrameworkConstants.hpp"
#define GAME_UPDATE_SPEED 165
#define FRAMES_BETWEEN_STAT_UPDATES 120
#define DISABLE_RUNTIME_CONSOLE 1

// do NOT increase this. it does NOT like to be changed easily
constexpr auto CHUNKSIZE = 32;

enum DefaultTileID {
	EMPTY,
	STONE,
	DIRT
};


enum class Corner {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
};


#endif