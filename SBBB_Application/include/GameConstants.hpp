#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

#include "Framework/FrameworkConstants.hpp"
#define GAME_UPDATE_SPEED 60
#define FRAMES_BETWEEN_STAT_UPDATES 60
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

struct DebugStats {
	float drawFPS;
	float updateFPS;
	float camX;
	float camY;
	float camFX1;
	float camFY1;
	float camFX2;
	float camFY2;
	int screenW;
	int screenH;
	int windowW;
	int windowH;
	int chunkCount;
	int emptyChunkCount;
	int drawnChunkCount;
	int noisemapTileCount;
	int drawCalls;
	bool statUpdate;

	// globals don't play nice
	static DebugStats& Get() {
		static DebugStats instance;
		return instance;
	}
};


#endif