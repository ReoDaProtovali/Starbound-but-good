#pragma once
#ifndef WORLD_H
#define WORLD_H


#include <stdlib.h>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

#include "Tile.hpp"
#include "Chunk.hpp"

#include <GL/glew.h>
#include <util/ext/glm/glm.hpp>

#include "Camera.hpp"
#include "Framework/Window/GameWindow.hpp"


class ChunkManager
{
public:
	// init the null chunk for functions that return a chunk that doesn't exist
	ChunkManager() : k_nullChunk(std::make_shared<WorldChunk>(WorldChunk())) {};

	bool genChunk(ChunkPos p_chunkPos);
	bool genChunk(int p_chunkX, int p_chunkY);
	void enqueueGen(ChunkPos p_chunkPos);
	bool genFromQueue();
	void genFixed(size_t x, size_t y);
	bool autoGen(Camera& p_cam);

	size_t findChunkIndex(ChunkPos p_chunkPos, bool& p_success);
	//WorldChunk& getChunk(ChunkPos p_chunkPos, bool& p_success);
	std::weak_ptr<WorldChunk> getChunkPtr(ChunkPos p_chunkPos, bool& p_success);

	// Will return chunks within the frame one by one until there are none more to fetch.
	// p_finished is used to terminate while loops.
	// make sure the chunks you are trying to use are not invalid
	// overcomplicated
	std::weak_ptr<WorldChunk> fetchFromFrame(glm::vec4 p_viewFrame, bool& p_finished);

	bool chunkExistsAt(ChunkPos p_chunkPos);
	bool removeChunk(ChunkPos p_chunkPos);
	void removeChunks();

	void draw(DrawSurface& p_target, DrawStates& p_drawStates);
	void logSize();
	int getChunkCount() { return (int)m_chunkList.size(); }
	int getEmptyChunkCount();
	void logChunks();

private:

	// For fetching chunks from a supplied frame
	bool m_setFetchCounterFlag = true;
	uint32_t m_fetchCounter = 0;
	bool m_doneFetching = false;

	std::vector<std::shared_ptr<WorldChunk>> m_chunkList;
	std::queue<ChunkPos> m_loadQueue;
	std::vector<ChunkPos> m_indices;

	// for returning when failed
	std::shared_ptr<WorldChunk> k_nullChunk;

};

#endif WORLD_H