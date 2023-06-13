#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

#include "Framework/FrameworkConstants.hpp"
#include "Globals.hpp"
#define POLLING_RATE_MS 10
#define UPDATE_RATE_FPS 60
#define FRAMES_BETWEEN_STAT_UPDATES 60
#define DISABLE_RUNTIME_CONSOLE false
//#define DISABLE_DEBUG_STATS

#define TILES_TO_METERS 0.5f
#define METERS_TO_TILES 2.0f
// do NOT increase this beyond limits. it does NOT like to be changed easily
// Currently works from 2-64
constexpr auto CHUNKSIZE = 64;
// Amount of world layers. Increasing heavily decreases chunk generation and vbo generation speed, also, kinda set in stone now. Things will break beyond 4.
// update: things also break under 4
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
struct KeyEvent {
	bool wasDown;
	int keyCode;
	bool valid = false; // for compatability for things that expect a conditional keyevent
};
struct MouseEvent {
	float x = 0.f;
	float y = 0.f;
	// specifically for gui
	float pixelX = 0.f;
	float pixelY = 0.f;
	bool wasClick = false;
	bool wasRelease = false;
	bool wasMove = false;
	uint32_t mouseState = 0;
};

enum class ChunkUpdateType {
	DONE_GENERATING,
	NEW_TILE_DATA,
	NEW_VBO_DATA,
	REMOVED
};
// just a unique struct for it because stoopid
struct ChunkUpdate {
	int x;
	int y;
	ChunkUpdateType type;
};

struct TileUpdateRequest {
	int x;
	int y;
	int z;
	int numericalID;
	bool operator==(const TileUpdateRequest& other) const {
		return other.x == x && other.y == y && other.z == z && other.numericalID == numericalID;
	}
};

#define TILESET_PATH "res\\tilesets"
#define GENERATORS_JSON_PATH ".\\src\\Shaders\\noisegenerators\\generators.json"

namespace Globals {
	inline bool shouldInterpolate() {
		return (float(UPDATE_RATE_FPS) / float(globals.refresh_rate)) < 0.8f; // 80% for now
	}
}

#endif