#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

#include "Framework/FrameworkConstants.hpp"
#include <atomic>
#include <thread>
#define POLLING_RATE_MS 10
#define UPDATE_RATE_FPS 60
#define FRAMES_BETWEEN_STAT_UPDATES 60
#define DISABLE_RUNTIME_CONSOLE false

#define TILES_TO_METERS 0.5f
#define METERS_TO_TILES 2.0f
// do NOT increase this beyond limits. it does NOT like to be changed easily
// Currently works from 2-64
constexpr auto CHUNKSIZE = 64;
// Amount of world layers. Increasing heavily decreases chunk generation and vbo generation speed.
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


//enum class ChunkPacketStatus {
//	VALID,
//	GENERATING,
//	UPDATE,
//	REMOVAL
//};
//
//struct ChunkPacket {
//	ChunkPacketStatus status;
//	ChunkPos pos;
//	WorldChunk* data;
//};

// just a unique struct for it because stoopid
struct ChunkUpdate {
	int x;
	int y;
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
	std::atomic<int> vertCount = 0;
	int loadQueueSize = 0;
	std::atomic<int> chunkGenCounter = 0;
	bool statUpdate = true;

	std::thread::id drawThread;

	// globals don't play nice
	static DebugStats& Get() {
		static DebugStats instance;
		return instance;
	}
};


#endif