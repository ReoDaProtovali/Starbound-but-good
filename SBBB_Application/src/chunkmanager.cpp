#include "ChunkManager.hpp"
#include <cstdio>

bool ChunkManager::genChunk(ChunkPos p_chunkPos)
{
	static auto allGenerators = res.getAllGeneratorShaders();
	for (auto& str : allGenerators) {
		m_noiseMap.genTilesNeighboringChunk(p_chunkPos.x, p_chunkPos.y, str);
	}
	if (chunkExistsAt(p_chunkPos)) return false;

	// add it to the back of the translation map
	WorldChunk c = WorldChunk(p_chunkPos, 0);


	c.worldGenerate(m_noiseMap);
	m_chunkMap[p_chunkPos] = c;// world ID is hardcoded for now. Will most def be a different system later.

	// fix borders, a little expensive but hey
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (j == 0 && i == 0) continue;
			if (chunkExistsAt(p_chunkPos.x + j, p_chunkPos.y + i)) {
				m_chunkMap[ChunkPos(p_chunkPos.x + j, p_chunkPos.y + i)].generateVBO(*this);
			}
		}
	}
	return true;
}
bool ChunkManager::genChunk(int p_chunkX, int p_chunkY)
{
	static auto allGenerators = res.getAllGeneratorShaders();
	for (auto& str : allGenerators) {
		m_noiseMap.genTilesNeighboringChunk(p_chunkX, p_chunkY, str);
	}
	if (chunkExistsAt(ChunkPos(p_chunkX, p_chunkY))) return false;

	// add it to the back of the translation map
	WorldChunk c = WorldChunk(ChunkPos(p_chunkX, p_chunkY), 0);
	c.worldGenerate(m_noiseMap);
	m_chunkMap[c.worldPos] = c;// world ID is hardcoded for now. Will most def be a different system later.

	// fix borders, a little expensive but hey
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (j == 0 && i == 0) continue;
			if (chunkExistsAt(p_chunkX + j, p_chunkY + i)) {
				m_chunkMap[ChunkPos(p_chunkX + j, p_chunkY + i)].generateVBO(*this);
			}
		}
	}
	return true;
}
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
		m_workCount.release();
		m_loadQueue.push(p_chunkPos);
	};
}

bool ChunkManager::genFromQueue()
{

	ChunkPos chunkPos;
	if (m_loadQueue.empty()) return false;

	//std::cout << loadQueue.size() << std::endl;
	chunkPos = m_loadQueue.front();
	m_loadQueue.pop();
	genChunk(chunkPos);
	updateDrawList(generatorCam->getFrame(), true);
	return true;
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

		ChunkPos pos = instance.m_loadQueue.front();
		instance.m_loadQueue.pop();
		lock.unlock();
		ChunkManager::genChunkThreaded(pos, instance);
	}
}
void ChunkManager::genChunkThreaded(ChunkPos p_chunkPos, ChunkManager& instance)
{
	std::unique_lock<std::mutex> lock(instance.m_chunkReadWriteMutex);
	if (instance.chunkExistsAt(p_chunkPos)) return;
	lock.unlock();
	// add it to the back of the translation map
	WorldChunk c = WorldChunk(p_chunkPos, 0);

	c.worldGenerate(instance.m_noiseMap);

	lock.lock();
	instance.m_chunkMap[p_chunkPos] = c;// world ID is hardcoded for now. Will most def be a different system later.

	// fix borders, a little expensive but hey
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (j == 0 && i == 0) continue;
			if (instance.chunkExistsAt(p_chunkPos.x + j, p_chunkPos.y + i)) {
				instance.m_chunkMap[ChunkPos(p_chunkPos.x + j, p_chunkPos.y + i)].generateVBO(instance);
			}
		}
	}
}
void ChunkManager::genFixed(size_t x, size_t y) {
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
//WorldChunk& ChunkManager::getChunk(ChunkPos p_chunkPos, bool& p_success) { // actually gets the chunk
//	bool found;
//	size_t index = findChunkIndex(p_chunkPos, found);
//	p_success = found;
//	// still kinda scuffed but it gotta return something
//	if (!found) return nullChunk;
//	return *chunkList[index];
//}
std::optional<WorldChunk*> ChunkManager::getChunkPtr(ChunkPos p_chunkPos)
{
	if (!chunkExistsAt(p_chunkPos)) return std::nullopt;
	return std::optional<WorldChunk*>(&m_chunkMap[p_chunkPos]);
}

glm::ivec4 frameToChunkCoords(glm::vec4 p_frame) {
	int x1 = utils::gridFloor((int)p_frame.x, CHUNKSIZE);
	int y1 = utils::gridFloor((int)p_frame.y, CHUNKSIZE) + 1;
	int x2 = utils::gridFloor((int)p_frame.z, CHUNKSIZE) + 1;
	int y2 = utils::gridFloor((int)p_frame.w, CHUNKSIZE) + 2;
	return glm::ivec4(x1, y1, x2, y2);
}
void ChunkManager::updateDrawList(glm::vec4 p_frame, bool force) {
	// TODO:: Make this smarter such that it doesn't need to rebuild the list completely when the frame moves
	static glm::ivec4 prev = glm::ivec4(0);
	glm::ivec4 next = frameToChunkCoords(p_frame);
	if (SDL_GetTicks() % 10 < 5) force = true;
	if ((prev == next) && !force) return;
	m_drawList.clear();
	int count = 0;
	auto it = m_drawList.before_begin();
	for (int y = next.y; y < next.w; y++) {
		for (int x = next.x; x < next.z; x++) {
			if (!chunkExistsAt(ChunkPos(x, y))) continue;
			it = m_drawList.insert_after(it, &m_chunkMap[ChunkPos(x, y)]);
			count++;
		}
	}
	prev = next;
}
int ChunkManager::drawVisible(DrawSurface& p_target, DrawStates& p_states, Shader& p_tileShader) {
	int count = 0;
	//return 0;
	for (auto chunk : m_drawList) {
		std::unique_lock<std::mutex> lock(m_chunkReadWriteMutex);
		if (!chunk) continue;
		if (chunk->isEmpty) continue;
		if (!chunk->vboIsPushed) {
			//chunk->generateVBO(*this);
			chunk->pushVBO();
		}

		// excuse my use of this here
		//glClear(GL_DEPTH_BUFFER_BIT);
		p_tileShader.setVec2Uniform(2, glm::vec2(chunk->worldPos.x, chunk->worldPos.y));
		chunk->draw(p_target, p_states);
		count++;
	}
	return count;
}
bool ChunkManager::chunkExistsAt(ChunkPos p_chunkPos) {
	auto it = m_chunkMap.find(p_chunkPos);
	if (it != m_chunkMap.end()) {
		if (it->second.invalid) return false;
		return true;
	}
	return false;
}
bool ChunkManager::chunkExistsAt(int p_chunkX, int p_chunkY) {
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

