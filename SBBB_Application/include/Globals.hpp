#pragma once
#include <atomic>
#include <thread>
#include "util/SubjectObserver.hpp"
struct DebugStats {
	float drawFPS = 0.f;
	float updateFPS = 0.f;
	float camX = 0.f;
	float camY = 0.f;
	float camFX1 = 0.f;
	float camFY1 = 0.f;
	float camFX2 = 0.f;
	float camFY2 = 0.f;
	float playerXVel = 0.f;
	float playerYVel = 0.f;
	float playerXAcc = 0.f;
	float playerYAcc = 0.f;
	int screenW = 0;
	int screenH = 0;
	int windowW = 0;
	int windowH = 0;
	int chunkCount = 0;
	int emptyChunkCount = 0;
	int drawnChunkCount = 0;
	int noisemapTileCount = 0;
	int drawCalls = 0;
	int loadQueueSize = 0;
	std::atomic<int> chunkGenCounter = 0;
	bool statUpdate = true;

	std::thread::id drawThread;
};

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
	uint8_t mouseButton = 0;
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

struct DebugStats;
struct GlobalStruct {
	Subject<MouseEvent> mouseSubject; // keeping these global fixes issues
	Subject<KeyEvent> keySubject;
	Subject<ChunkUpdate> chunkUpdateSubject;
	Subject<TileUpdateRequest> tileUpdateRequestSubject;
	DebugStats debug;
	float updateFPS = 0.f;
	float drawFPS = 0.f;
	int refresh_rate = 0;
	bool worldDoneGenerating = false;
};
extern GlobalStruct globals;