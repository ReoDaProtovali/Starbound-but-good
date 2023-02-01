#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

#include "Framework/FrameworkConstants.hpp"
#include <atomic>
#define GAME_UPDATE_SPEED 60
#define FRAMES_BETWEEN_STAT_UPDATES 60
#define DISABLE_RUNTIME_CONSOLE false

// do NOT increase this beyond limits. it does NOT like to be changed easily
// Currently works from 2-64
constexpr auto CHUNKSIZE = 64;
constexpr auto CHUNKDEPTH = 4;
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

#define TILESET_PATH "res\\tilesets"
#define GENERATORS_JSON_PATH ".\\src\\Shaders\\noisegenerators\\generators.json"


struct DebugStats {
	float drawFPS = 0.f;
	float updateFPS = 0.f;
	float camX = 0.f;
	float camY = 0.f;
	float camFX1 = 0.f;
	float camFY1 = 0.f;
	float camFX2 = 0.f;
	float camFY2 = 0.f;
	int screenW = 0;
	int screenH = 0;
	int windowW = 0;
	int windowH = 0;
	int chunkCount = 0;
	int emptyChunkCount = 0;
	int drawnChunkCount = 0;
	int noisemapTileCount = 0;
	int drawCalls = 0;
	int vertCount = 0;
	int loadQueueSize = 0;
	std::atomic<int> chunkGenCounter = 0;
	bool statUpdate = true;

	// globals don't play nice
	static DebugStats& Get() {
		static DebugStats instance;
		return instance;
	}
};


#endif