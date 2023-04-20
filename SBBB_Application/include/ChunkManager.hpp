#pragma once

#include <stdlib.h>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>
#include <optional>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <semaphore>

#include "Tile.hpp"
#include "Chunk.hpp"

#include <GL/glew.h>
#include <util/ext/glm/glm.hpp>

#include "Camera.hpp"
#include "Framework/Window/GameWindow.hpp"
#include "util/Messenger.hpp"
#include "util/SharedMap.hpp"
#include "util/SharedQueue.hpp"
#include "WorldGenNoisemap.hpp"
#include "WorldGenerator.hpp"

#include "util/SubjectObserver.hpp"
#define GENERATION_THREAD_COUNT 4

class ChunkManager
{
public:
	// Basic init, reserving a block of memory for future chunks.
	ChunkManager() { s_chunkMap.reserve(1024); };

	void regenVBOs();
	void flip();
	void enqueueGen(ChunkPos p_chunkPos);
	void genFixed(int x, int y, uint32_t w, uint32_t h);
	void processRequests();
	void tidyNoisemapIfDone();

	void startThreads();
	void stopThreads();
	void genFromQueueThreaded();
	void genChunkThreaded(ChunkPos p_chunkPos);

	std::optional<WorldChunk*> getChunkPtr(ChunkPos p_chunkPos);
	void setCollisionWorld(b2World* p_world);
	void generateColliders();

	// tells you if a map entry has been made
	bool chunkExistsAt(ChunkPos p_chunkPos);
	// tells you if a map entry has been made
	bool chunkExistsAt(int p_chunkX, int p_chunkY);
	// tells you if a chunk has actually been generated
	bool validChunkExistsAt(ChunkPos p_chunkPos);
	// tells you if a chunk has actually been generated
	bool validChunkExistsAt(int p_chunkX, int p_chunkY);
	bool removeChunk(ChunkPos p_chunkPos);
	void removeChunks();

	void logSize();
	int getChunkCount() { return (int)s_chunkMap.size(); }
	int getEmptyChunkCount();
	void logChunks();

private:
	WorldGenerator m_worldgen;
	ResourceManager& res = ResourceManager::Get();

	std::vector<std::thread> m_genThreads;
	std::mutex m_chunkVBOMutex;
	std::atomic<bool> m_stopAllThreads = false;
	DebugStats& stats = DebugStats::Get();
	
	Messenger<ChunkPos, int>& s_generationRequest = Messenger<ChunkPos, int>::Get();
	SharedMap<ChunkPos, WorldChunk, ChunkPos>& s_chunkMap = SharedMap<ChunkPos, WorldChunk, ChunkPos>::Get();
	SharedQueue<ChunkPos> m_loadQueue;
	SharedQueue<int> m_generatingQueue; // just a list for the generator threads to say "hey, don't delete the noisemap data!"

	Subject<ChunkUpdate>& m_chunkUpdateSubject = Subject<ChunkUpdate>::Get();
	Observer<ChunkUpdate> m_updateObserver; // not insane, because the main server thread is just as clueless about when this stuff happens

	b2World* m_collisionWorldPtr = nullptr;
};
