#include "ChunkManager.hpp"
#include <cstdio>

void ChunkManager::regenVBOs()
{
	for (auto& it : m_chunkMap) {
		it.second.generateVBO(*this);
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
		// because this is a map, it inserts a default constructed chunk when you do this
		m_chunkMap[p_chunkPos];
		m_workCount.release();
	};
}

bool ChunkManager::autoGen(Camera& p_cam) {
	// scuffed
	//for (int i = (int)(p_cam.getFrame().y / (float)CHUNKSIZE) - 2;
	//	i < (int)((p_cam.getFrame().w) / (float)CHUNKSIZE) + 2;
	//	i++) {
	//	for (int j =
	//		(int)(p_cam.getFrame().x / (float)CHUNKSIZE) - 2;
	//		j < (int)((p_cam.getFrame().z) / (float)CHUNKSIZE) + 2;
	//		j++) {
	//		if ((i > -16) && (i < 16) && (j > -40) && (j < 40)) {
	//			ChunkManager::enqueueGen(ChunkPos(j, i));
	//		}
	//	}
	//}
	return false;
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
		std::unique_lock<std::mutex> lock(instance.m_queueMutex);

		instance.m_workCount.acquire();
		if (instance.m_loadQueue.empty()) continue;

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
	std::unique_lock<std::mutex> lock(instance.m_chunkReadWriteMutex);
	c.worldGenerate(instance.m_noiseMap);
	instance.m_chunkMap[p_chunkPos] = std::move(c);
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (j == 0 && i == 0) continue;
			if (instance.validChunkExistsAt(p_chunkPos.x + j, p_chunkPos.y + i)) {
				instance.m_chunkMap[ChunkPos(p_chunkPos.x + j, p_chunkPos.y + i)].generateVBO(instance);
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
	printf("Chunk count: %zu\n", m_chunkMap.size());
}
int ChunkManager::getEmptyChunkCount()
{
	int count = 0;
	for (auto& it : m_chunkMap) {
		count += (int)it.second.isEmpty;
	}
	return count;
}
void ChunkManager::logChunks() {

	for (auto& it : m_chunkMap) {
		printf("There is a chunk at position %i %i\n", it.second.worldPos.x, it.second.worldPos.y);
	}
}

std::optional<WorldChunk*> ChunkManager::getChunkPtr(ChunkPos p_chunkPos)
{
	if (!validChunkExistsAt(p_chunkPos)) return std::nullopt;
	return std::optional<WorldChunk*>(&m_chunkMap[p_chunkPos]);
}

int ChunkManager::drawChunkFrame(int p_x1, int p_y1, int p_x2, int p_y2, DrawSurface& p_target, DrawStates& p_states, Shader& p_tileShader) {

	int drawnCount = 0;
	std::unique_lock<std::mutex> lock(m_chunkReadWriteMutex);
	for (int y = p_y1; y <= p_y2; y++) {
		for (int x = p_x1; x <= p_x2; x++) {
			if (!validChunkExistsAt(ChunkPos(x, y))) continue;
			WorldChunk& chunk = m_chunkMap[ChunkPos(x, y)];
			if (chunk.isEmpty) continue;
			if (!chunk.vboIsPushed) {
				chunk.pushVBO();
			}

			p_tileShader.setVec2Uniform(2, glm::vec2(chunk.worldPos.x, chunk.worldPos.y));
			chunk.draw(p_target, p_states);
			drawnCount++;
		}
	}
	return drawnCount;
}
bool ChunkManager::chunkExistsAt(ChunkPos p_chunkPos) {
	auto it = m_chunkMap.find(p_chunkPos);
	if (it != m_chunkMap.end()) {
		return true;
	}
	return false;
}
bool ChunkManager::chunkExistsAt(int p_chunkX, int p_chunkY) {
	auto it = m_chunkMap.find(ChunkPos(p_chunkX, p_chunkY));
	if (it != m_chunkMap.end()) {
		return true;
	}
	return false;
}
bool ChunkManager::validChunkExistsAt(ChunkPos p_chunkPos) {
	auto it = m_chunkMap.find(p_chunkPos);
	if (it != m_chunkMap.end()) {
		if (it->second.invalid) return false;
		return true;
	}
	return false;
}
bool ChunkManager::validChunkExistsAt(int p_chunkX, int p_chunkY) {
	auto it = m_chunkMap.find(ChunkPos(p_chunkX, p_chunkY));
	if (it != m_chunkMap.end()) {
		if (it->second.invalid) return false;
		return true;
	}
	return false;
}

bool ChunkManager::removeChunk(ChunkPos p_chunkPos)
{
	return (bool)m_chunkMap.erase(p_chunkPos);
}

void ChunkManager::removeChunks()
{
	m_chunkMap.clear();
}

