#pragma once
#include <atomic>
#include <thread>

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
struct DebugStats;
struct GlobalStruct {
	DebugStats debug;
	float updateFPS = 0.f;
	float drawFPS = 0.f;
	int refresh_rate = 0;
	bool worldDoneGenerating = false;
};
extern GlobalStruct globals;