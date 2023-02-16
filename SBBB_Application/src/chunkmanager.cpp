#include "ChunkManager.hpp"
#include <cstdio>

void ChunkManager::regenVBOs()
{
	for (auto& it : s_chunkMap) {
		it.second.generateVBO(*this);
	}
}
void ChunkManager::flip()
{
	for (auto& it : s_chunkMap) {
		it.second.flip();
	}
}
void ChunkManager::enqueueGen(ChunkPos p_chunkPos)
{
	std::unique_lock<std::mutex> lock(m_queueMutex);
	if (!chunkExistsAt(p_chunkPos)) {
		static auto allGenerators = res.getAllGeneratorShaders();
		for (auto& str : allGenerators) {
			m_noiseMap.genTilesNeighboringChunk(p_chunkPos.x, p_chunkPos.y, str);
		}
		m_loadQueue.push(p_chunkPos);
		// because this is a std::unordered_map, it inserts a default constructed chunk when you do this
		s_chunkMap[p_chunkPos];
		m_workCount.release();
	};
}

void ChunkManager::processRequests() {

}
void ChunkManager::startThreads()
{
	if (m_genThreads.size() > 0) return;
	for (int i = 0; i < GENERATION_THREAD_COUNT; i++) {
		m_genThreads.emplace_back(&ChunkManager::genFromQueueThreaded, std::ref(*this));
	}
}
void ChunkManager::stopThreads()
{
	m_stopAllThreads = true;
	m_workCount.release(GENERATION_THREAD_COUNT);
	for (auto& t : m_genThreads) {
		t.join();
	}
}
void ChunkManager::genFromQueueThreaded(ChunkManager& instance)
{
	while (true) {
		if (instance.m_stopAllThreads) break;

		instance.m_workCount.acquire();
		std::unique_lock<std::mutex> lock(instance.m_queueMutex);
		if (instance.m_loadQueue.empty()) {
			lock.unlock();
			continue;
		};

		// enable this if you don't want to have to move the camera to see new chunks
		//instance.notifyNewChunk = true;
		ChunkPos pos = instance.m_loadQueue.front();
		instance.m_loadQueue.pop();
		lock.unlock();
		ChunkManager::genChunkThreaded(pos, instance);
	}
}
void ChunkManager::genChunkThreaded(ChunkPos p_chunkPos, ChunkManager& instance)
{

	WorldChunk c = WorldChunk(p_chunkPos, 0); // world ID is hardcoded for now. Will most def be a different system later.

	c.worldGenerate(instance.m_noiseMap);
	instance.s_chunkMap[p_chunkPos] = std::move(c);
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			//if (j == 0 && i == 0) continue;
			auto& neighbor = instance.s_chunkMap[ChunkPos(p_chunkPos.x + j, p_chunkPos.y + i)];
			if (instance.validChunkExistsAt(p_chunkPos.x + j, p_chunkPos.y + i)) {
				neighbor.drawable = false;
				neighbor.generateVBO(instance);
				neighbor.drawable = true;
			}
		}
	}

}
void ChunkManager::genFixed(uint32_t x, uint32_t y) {
	for (size_t i = 0; i < y; i++) {
		for (size_t j = 0; j < x; j++) {
			enqueueGen(ChunkPos((int)j - x / 2, (int)i - y / 2));
		}
	}
}
void ChunkManager::logSize() {
	printf("Chunk count: %zu\n", s_chunkMap.size());
}
int ChunkManager::getEmptyChunkCount()
{
	int count = 0;
	for (auto& it : s_chunkMap) {
		count += (int)it.second.isEmpty;
	}
	return count;
}
void ChunkManager::logChunks() {

	for (auto& it : s_chunkMap) {
		printf("There is a chunk at position %i %i\n", it.second.worldPos.x, it.second.worldPos.y);
	}
}


std::optional<WorldChunk*> ChunkManager::getChunkPtr(ChunkPos p_chunkPos)
{
	if (!validChunkExistsAt(p_chunkPos)) return std::nullopt;
	return std::optional<WorldChunk*>(&s_chunkMap[p_chunkPos]);
}

bool ChunkManager::chunkExistsAt(ChunkPos p_chunkPos) {
	return s_chunkMap.contains(p_chunkPos);
}
bool ChunkManager::chunkExistsAt(int p_chunkX, int p_chunkY) {
	return s_chunkMap.contains(ChunkPos(p_chunkX, p_chunkY));
}
bool ChunkManager::validChunkExistsAt(ChunkPos p_chunkPos) {
	auto it = s_chunkMap.find(p_chunkPos);
	if (it != s_chunkMap.end()) {
		if (it->second.invalid) return false;
		return true;
	}
	return false;
}
bool ChunkManager::validChunkExistsAt(int p_chunkX, int p_chunkY) {
	auto it = s_chunkMap.find(ChunkPos(p_chunkX, p_chunkY));
	if (it != s_chunkMap.end()) {
		if (it->second.invalid) return false;
		return true;
	}
	return false;
}

bool ChunkManager::removeChunk(ChunkPos p_chunkPos)
{
	return (bool)s_chunkMap.erase(p_chunkPos);
}

void ChunkManager::removeChunks()
{
	s_chunkMap.clear();
}

