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
	if (!chunkExistsAt(p_chunkPos)) {
		static auto allGenerators = res.getAllGeneratorShaders();
		for (auto& str : allGenerators) {
			//m_noiseMap.genTilesNeighboringChunk(p_chunkPos.x, p_chunkPos.y, str);
			m_worldgen.loadNoiseValuesAtChunk(p_chunkPos.x, p_chunkPos.y);
		}
		m_generatingQueue.push(0);
		m_loadQueue.push(p_chunkPos);
		// because this is a std::unordered_map, it inserts a default constructed chunk when you do this
		s_chunkMap[p_chunkPos];
	};
}

void ChunkManager::processRequests() {
	static Observer<KeyEvent> keyPresses;
	while (auto opt = keyPresses.observe()) {
		switch (opt.value().keyCode) {
		case SDLK_4:
			regenVBOs();
			break;
		}
	}
	if (auto opt = s_generationRequest.getMessageFront()) {
		enqueueGen(ChunkPos(opt.value().x, opt.value().y));
	}
}

void ChunkManager::tidyNoisemapIfDone() {
	if (m_generatingQueue.length() == 0) {
		m_worldgen.clearNoisemap();
		globals.worldDoneGenerating = true;
	}
}
void ChunkManager::startThreads()
{
	if (m_genThreads.size() > 0) return;
	for (int i = 0; i < GENERATION_THREAD_COUNT; i++) {
		m_genThreads.emplace_back(&ChunkManager::genFromQueueThreaded, this);
	}
}
void ChunkManager::stopThreads()
{
	m_stopAllThreads = true;

	// force close waiting threads
	m_loadQueue.forceAllThreadsToPop();

	for (auto& t : m_genThreads) {
		t.join();
	}
}
void ChunkManager::genFromQueueThreaded()
{
	while (true) {
		ChunkPos pos = m_loadQueue.pop();

		// must be done here, because if it terminates, the position from .pop() is invalid 
		if (m_stopAllThreads) break;

		ChunkManager::genChunkThreaded(pos);
		m_generatingQueue.pop();
	}
}
void ChunkManager::genChunkThreaded(ChunkPos p_chunkPos)
{

	WorldChunk c = WorldChunk(p_chunkPos, 0); // world ID is hardcoded for now. Will most def be a different system later.

	// assume valid noisemap at position
	bool empty = true;
	c.setTiles(m_worldgen.generateChunk(p_chunkPos.x, p_chunkPos.y, empty));
	stats.chunkCount++;
	stats.chunkGenCounter++;
	if (empty) stats.emptyChunkCount++;

	c.isEmpty = empty;
	c.invalid = false;

	s_chunkMap[p_chunkPos] = std::move(c);
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			//if (j == 0 && i == 0) continue;
			auto& neighbor = s_chunkMap[ChunkPos(p_chunkPos.x + j, p_chunkPos.y + i)];
			if (validChunkExistsAt(p_chunkPos.x + j, p_chunkPos.y + i)) {
				neighbor.drawable = false;
				neighbor.generateVBO(*this);
				neighbor.drawable = true;
				m_chunkUpdateSubject.notifyAll(ChunkUpdate(p_chunkPos.x + j, p_chunkPos.y + i, ChunkUpdateType::NEW_VBO_DATA));
			}
		}
	}
	m_chunkUpdateSubject.notifyAll(ChunkUpdate(p_chunkPos.x, p_chunkPos.y, ChunkUpdateType::DONE_GENERATING));
}
void ChunkManager::genFixed(int x, int y, uint32_t w, uint32_t h) {
	for (size_t i = 0; i < h; i++) {
		for (size_t j = 0; j < w; j++) {
			enqueueGen(ChunkPos((int)j + x, (int)i + y));
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

void ChunkManager::setCollisionWorld(b2World* p_world)
{
	m_collisionWorldPtr = p_world;
}

void ChunkManager::generateColliders()
{
	while (auto opt = m_updateObserver.observe()) {
		if (opt.value().type == ChunkUpdateType::DONE_GENERATING || opt.value().type == ChunkUpdateType::NEW_TILE_DATA) {
			WorldChunk& c = s_chunkMap[ChunkPos(opt.value().x, opt.value().y)];
			if (c.invalid || c.isEmpty) return;
			c.genCollider(*m_collisionWorldPtr, *this);
		}
	}
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

