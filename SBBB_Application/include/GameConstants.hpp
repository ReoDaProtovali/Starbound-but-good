#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

#include "Framework/FrameworkConstants.hpp"
#define GAME_UPDATE_SPEED 60
#define FRAMES_BETWEEN_STAT_UPDATES 120
#define DISABLE_RUNTIME_CONSOLE false

// do NOT increase this beyond limits. it does NOT like to be changed easily
// Currently works from 2-64
constexpr auto CHUNKSIZE = 64;

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