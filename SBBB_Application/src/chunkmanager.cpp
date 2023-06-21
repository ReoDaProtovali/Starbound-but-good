#include "ChunkManager.hpp"
#include <cstdio>
#include <set>
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
	static Observer<KeyEvent> keyPresses{ globals.keySubject };
	while (auto opt = s_generationRequest.getMessageFront()) {
		enqueueGen(ChunkPos(opt.value().x, opt.value().y));
	}

	// shenanigans
	TileUpdateRequest lastReq{ 999, 999, 999, -1 };
	while (auto opt = m_tileRequests.observe()) {
		if (lastReq.numericalID == -1) lastReq = opt.value();
		if (opt.value() == lastReq) continue;
		setTile(lastReq.numericalID, lastReq.x, lastReq.y, lastReq.z);
		lastReq = opt.value();
	}
	if (lastReq.numericalID != -1) {
		setTile(lastReq.numericalID, lastReq.x, lastReq.y, lastReq.z);
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
	LOAD_LOG("ChunkManager generation threads started...");
	if (m_genThreads.size() > 0) return;
	for (int i = 0; i < GENERATION_THREAD_COUNT; i++) {
		m_genThreads.emplace_back(&ChunkManager::genFromQueueThreaded, this);
	}
	for (int i = 0; i < VBO_THREAD_COUNT; i++) {
		m_vboThreads.emplace_back(&ChunkManager::genVBOFromQueueThreaded, this);
	}
}
void ChunkManager::stopThreads()
{
	LOAD_LOG("ChunkManager generation threads stopping...");
	m_stopAllThreads = true;

	// force close waiting threads
	m_loadQueue.forceAllThreadsToPop();
	m_vboQueue.forceAllThreadsToPop();
	for (auto& t : m_genThreads) {
		t.join();
	}
	for (auto& t : m_vboThreads) {
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
void ChunkManager::genVBOFromQueueThreaded()
{
	while (true) {
		ChunkPos pos = m_vboQueue.pop();
		// must be done here, because if it terminates, the position from .pop() is invalid 
		if (m_stopAllThreads) break;
		if (!validChunkExistsAt(pos)) continue;

		s_chunkMap[pos].generateVBO(*this);
		s_chunkMap[pos].drawable = true;
		m_chunkUpdateSubject.notifyAll(ChunkUpdate(pos.x, pos.y, ChunkUpdateType::NEW_VBO_DATA));
	}
}
void ChunkManager::genChunkThreaded(ChunkPos p_chunkPos)
{

	WorldChunk c = WorldChunk(p_chunkPos, 0); // world ID is hardcoded for now. Will most def be a different system later.

	// assume valid noisemap at position
	bool empty = true;
	c.setTiles(m_worldgen.generateChunk(p_chunkPos.x, p_chunkPos.y, empty));
#ifndef DISABLE_DEBUG_STATS
	globals.debug.chunkCount++;
	globals.debug.chunkGenCounter++;
	if (empty) globals.debug.emptyChunkCount++;
#endif
	c.isEmpty = empty;
	c.invalid = false;

	s_chunkMap[p_chunkPos] = std::move(c);
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			//if (j == 0 && i == 0) continue;
			auto& neighbor = s_chunkMap[ChunkPos(p_chunkPos.x + j, p_chunkPos.y + i)];
			if (validChunkExistsAt(p_chunkPos.x + j, p_chunkPos.y + i)) {
				//neighbor.drawable = false;
				m_vboQueue.push(ChunkPos(p_chunkPos.x + j, p_chunkPos.y + i));
				//neighbor.generateVBO(*this);
				//neighbor.drawable = true;
				//m_chunkUpdateSubject.notifyAll(ChunkUpdate(p_chunkPos.x + j, p_chunkPos.y + i, ChunkUpdateType::NEW_VBO_DATA));
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
	static std::set<ChunkPos> seenPositions;
	while (auto opt = m_updateObserver.observe()) {
		if (opt.value().type == ChunkUpdateType::DONE_GENERATING || opt.value().type == ChunkUpdateType::NEW_TILE_DATA) {

			WorldChunk& c = s_chunkMap[ChunkPos(opt.value().x, opt.value().y)];
			if (c.invalid || c.isEmpty) continue;
			seenPositions.insert(ChunkPos(opt.value().x, opt.value().y));
			break;
		}
	}
	for (ChunkPos pos : seenPositions) {
		WorldChunk& c = s_chunkMap[pos];
		c.genCollider(*m_collisionWorldPtr, *this);
	}
	seenPositions.clear();
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

void ChunkManager::setTile(const std::string& p_tileID, int p_worldX, int p_worldY, int p_worldLayer)
{
	int chunkX = utils::gridFloor(p_worldX, CHUNKSIZE);
	int chunkY = utils::gridFloor(p_worldY, CHUNKSIZE) + 1;
	if (!s_chunkMap.contains(ChunkPos(chunkX, chunkY))) return;
	if (s_chunkMap[ChunkPos(chunkX, chunkY)].invalid) return;
	WorldChunk& c = s_chunkMap[ChunkPos(chunkX, chunkY)];
	int localTileX = utils::modUnsigned(p_worldX, CHUNKSIZE);
	int localTileY = utils::modUnsigned(-p_worldY - 1, CHUNKSIZE);

	auto tileInfoOpt = res.getTileInfo(p_tileID);
	if (!tileInfoOpt.has_value()) throw std::exception("tile ID not found");

	//c(localTileX, localTileY, p_worldLayer).m_tileID = tileInfoOpt.value().get().spriteIndex;
	c.setChunkTile(glm::ivec3(localTileX, localTileY, p_worldLayer), tileInfoOpt.value().get().spriteIndex);
		if (tileInfoOpt.value().get().spriteIndex != 0) c.isEmpty = false;
	m_chunkUpdateSubject.notifyAll(ChunkUpdate(c.worldPos.x, c.worldPos.y, ChunkUpdateType::NEW_TILE_DATA));

	c.genSingleTileVBO(localTileX, localTileY, p_worldLayer, *this);
	//c.generateVBO(*this);
	m_chunkUpdateSubject.notifyAll(ChunkUpdate(c.worldPos.x, c.worldPos.y, ChunkUpdateType::NEW_VBO_DATA));
}

void ChunkManager::setTile(int p_tileID, int p_worldX, int p_worldY, int p_worldLayer)
{
	int chunkX = utils::gridFloor(p_worldX, CHUNKSIZE);
	int chunkY = utils::gridFloor(p_worldY, CHUNKSIZE) + 1;
	if (!s_chunkMap.contains(ChunkPos(chunkX, chunkY))) return;
	if (s_chunkMap[ChunkPos(chunkX, chunkY)].invalid) return;
	WorldChunk& c = s_chunkMap[ChunkPos(chunkX, chunkY)];
	int localTileX = utils::modUnsigned(p_worldX, CHUNKSIZE);
	int localTileY = utils::modUnsigned(-p_worldY - 1, CHUNKSIZE);


	//c(localTileX, localTileY, p_worldLayer).m_tileID = p_tileID;
	if (c.getChunkTile(localTileX, localTileY, p_worldLayer).m_tileID == p_tileID) return;
	c.setChunkTile(glm::ivec3(localTileX, localTileY, p_worldLayer), p_tileID);
	if (p_tileID != 0) c.isEmpty = false;
	m_chunkUpdateSubject.notifyAll(ChunkUpdate(c.worldPos.x, c.worldPos.y, ChunkUpdateType::NEW_TILE_DATA));

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			c.genSingleTileVBO(localTileX + j, localTileY + i, p_worldLayer, *this);
		}
	}
	m_chunkUpdateSubject.notifyAll(ChunkUpdate(c.worldPos.x, c.worldPos.y, ChunkUpdateType::NEW_VBO_DATA));
}

