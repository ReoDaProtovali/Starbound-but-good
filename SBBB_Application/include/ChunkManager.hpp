#pragma once

#include <stdlib.h>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>
#include <optional>
#include <forward_list>
#include <unordered_map>

#include "Tile.hpp"
#include "Chunk.hpp"

#include <GL/glew.h>
#include <util/ext/glm/glm.hpp>

#include "Camera.hpp"
#include "Framework/Window/GameWindow.hpp"
#include "WorldGenNoisemap.hpp"

class ChunkManager
{
public:
	// Basic init, reserving a block of memory for future chunks.
	ChunkManager() { m_chunkMap.reserve(1024); };

	bool genChunk(ChunkPos p_chunkPos);
	bool genChunk(int p_chunkX, int p_chunkY);
	void regenVBOs();
	void enqueueGen(ChunkPos p_chunkPos);
	bool genFromQueue();
	void genFixed(size_t x, size_t y);
	bool autoGen(Camera& p_cam);

	//WorldChunk& getChunk(ChunkPos p_chunkPos, bool& p_success);
	std::optional<WorldChunk*> getChunkPtr(ChunkPos p_chunkPos, bool& p_success);

	// Will return chunks within the frame one by one until there are none more to fetch.
	// p_finished is used to terminate while loops.
	// make sure the chunks you are trying to use are not invalid
	// overcomplicated
	std::optional<WorldChunk*> fetchFromFrame(glm::vec4 p_viewFrame, bool& p_finished);

	void updateDrawList(glm::vec4 p_frame);
	int drawVisible(DrawSurface& p_target, DrawStates& p_states, Shader& p_tileShader);

	bool chunkExistsAt(ChunkPos p_chunkPos);
	bool removeChunk(ChunkPos p_chunkPos);
	void removeChunks();

	//void draw(DrawSurface& p_target, DrawStates& p_drawStates);
	void logSize();
	int getChunkCount() { return (int)m_chunkMap.size(); }
	int getEmptyChunkCount();
	void logChunks();

private:
	WorldGenNoisemap m_noiseMap;
	ResourceManager& res = ResourceManager::Get();
	// For fetching chunks from a supplied frame
	bool m_setFetchCounterFlag = true;
	uint32_t m_fetchCounter = 0;
	bool m_doneFetching = false;

	std::unordered_map<ChunkPos, WorldChunk, ChunkPos> m_chunkMap;
	std::queue<ChunkPos> m_loadQueue;
	std::forward_list<WorldChunk*> m_drawList;
	//std::vector<ChunkPos> m_indices;
};
