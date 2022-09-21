#pragma once
#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "Tile.hpp"
#include "Chunk.hpp"
#include <stdlib.h>
#include <iostream>
//#include "util/utils.hpp"
#include <util/ext/glm/glm.hpp>
#include "Camera.hpp"
#include "Framework/Window/GameWindow.hpp"
#include <GL/glew.h>
#include <memory>
#include <queue>

class ChunkManager
{
public:
	ChunkManager() { 
		nullChunk = std::make_shared<WorldChunk>(WorldChunk()); 
	};
	bool genChunk(ChunkPos p_chunkPos);
	bool genChunk(int p_chunkX, int p_chunkY);
	void enqueueGen(ChunkPos p_chunkPos);
	void genFromQueue();
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
	int getChunkCount() { return chunkList.size(); }
	int getEmptyChunkCount();
	void logChunks();

private:

	// For fetching chunks from a supplied frame
	bool setFetchCounterFlag = true;
	unsigned int fetchCounter = 0;
	bool doneFetching = false;

	std::vector<std::shared_ptr<WorldChunk>> chunkList;
	std::queue<ChunkPos> loadQueue;
	std::vector<ChunkPos> indices;

	// for returning when failed
	std::shared_ptr<WorldChunk> nullChunk;

};

#endif WORLD_H