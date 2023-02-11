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
#include "WorldGenNoisemap.hpp"

#define GENERATION_THREAD_COUNT 4

class ChunkManager
{
public:
	// Basic init, reserving a block of memory for future chunks.
	ChunkManager() { m_chunkMap.reserve(1024); };

	//bool genChunk(ChunkPos p_chunkPos);
	//bool genChunk(int p_chunkX, int p_chunkY);
	void regenVBOs();
	void flip();
	void enqueueGen(ChunkPos p_chunkPos);
	//bool genFromQueue();
	void genFixed(uint32_t x, uint32_t y);
	bool autoGen(Camera& p_cam);

	void startThreads();
	void stopThreads();
	static void genFromQueueThreaded(ChunkManager& instance);
	static void genChunkThreaded(ChunkPos p_chunkPos, ChunkManager& instance);

	std::optional<WorldChunk*> getChunkPtr(ChunkPos p_chunkPos);

	void processRequests();
	//void updateDrawList(glm::vec4 p_frame, bool force = false);
	//int drawVisible(DrawSurface& p_target, DrawStates& p_states, Shader& p_tileShader);
	int drawChunkFrame(int p_x, int p_y, int p_w, int p_h, DrawSurface& p_target, DrawStates& p_states, Shader& p_tileShader);

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
	int getChunkCount() { return (int)m_chunkMap.size(); }
	int getEmptyChunkCount();
	void logChunks();

	Camera* generatorCam = nullptr;
	std::atomic<bool> notifyNewChunk = true;
private:
	WorldGenNoisemap m_noiseMap;
	ResourceManager& res = ResourceManager::Get();

	std::vector<std::thread> m_genThreads;
	std::mutex m_queueMutex;
	//std::mutex m_chunkReadWriteMutex;
	std::atomic<bool> m_stopAllThreads = false;
	std::counting_semaphore<> m_workCount{0};

	SharedMap<ChunkPos, WorldChunk, ChunkPos> m_chunkMap;
	std::queue<ChunkPos> m_loadQueue;
	Messenger<ChunkPos, WorldChunk*>& m_chunkMessenger = Messenger<ChunkPos, WorldChunk*>::Get();
	//std::forward_list<WorldChunk*> m_drawList;
	//std::vector<WorldChunk*> m_drawList;

};
