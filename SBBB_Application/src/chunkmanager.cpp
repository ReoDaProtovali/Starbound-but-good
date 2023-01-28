#include "ChunkManager.hpp"
#include <cstdio>

bool ChunkManager::genChunk(ChunkPos p_chunkPos)
{
	if (chunkExistsAt(p_chunkPos)) return false;

	// add it to the back of the translation map
	WorldChunk c = WorldChunk(p_chunkPos, 0);
	c.worldGenerate(m_noiseMap);
	m_chunkMap[p_chunkPos] = c;// world ID is hardcoded for now. Will most def be a different system later.

	return true;
}
bool ChunkManager::genChunk(int p_chunkX, int p_chunkY)
{
	if (chunkExistsAt(ChunkPos(p_chunkX, p_chunkY))) return false;

	// add it to the back of the translation map
	WorldChunk c = WorldChunk(ChunkPos(p_chunkX, p_chunkY), 0);
	c.worldGenerate(m_noiseMap);
	m_chunkMap[c.worldPos] = c;// world ID is hardcoded for now. Will most def be a different system later.
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
	//std::cout << chunkExistsAt(ChunkPos(0, 0)) << std::endl;
	//std::cout << loadQueue.size() << std::endl;
	if (!ChunkManager::chunkExistsAt(p_chunkPos)) {
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
	return true;
}

bool ChunkManager::autoGen(Camera& p_cam) {
	// scuffed
	for (int i = (int)(p_cam.getFrame().y / (float)CHUNKSIZE) - 2;
		i < (int)((p_cam.getFrame().w) / (float)CHUNKSIZE) + 2;
		i++) {
		for (int j =
			(int)(p_cam.getFrame().x / (float)CHUNKSIZE) - 2;
			j < (int)((p_cam.getFrame().z) / (float)CHUNKSIZE) + 2;
			j++) {
			if ((i > -16) && (i < 16) && (j > -40) && (j < 40)) {
				ChunkManager::enqueueGen(ChunkPos(j, i));
			}
		}
	}
	return false;
}
void ChunkManager::genFixed(size_t x, size_t y) {
	for (size_t i = 0; i < y; i++) {
		for (size_t j = 0; j < x; j++) {
			ChunkManager::enqueueGen(ChunkPos(j, i));
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
std::optional<WorldChunk*> ChunkManager::getChunkPtr(ChunkPos p_chunkPos, bool& p_success)
{
	bool success = chunkExistsAt(p_chunkPos);
	if (!p_success) return std::nullopt;
	return std::optional<WorldChunk*>(&m_chunkMap[p_chunkPos]);
}
std::optional<WorldChunk*> ChunkManager::fetchFromFrame(glm::vec4 p_viewFrame, bool& p_finished)
{
	// Frame should never change between calls to this function, always use within a while loop
	const float x = p_viewFrame.x - (float)CHUNKSIZE, y = p_viewFrame.y + (float)CHUNKSIZE, z = p_viewFrame.z + (float)CHUNKSIZE, w = p_viewFrame.w + (float)CHUNKSIZE;
	const float tileWidth = z - x + (float)CHUNKSIZE;
	const float tileHeight = w - y + (float)CHUNKSIZE;
	// Width and height, in chunks. Always positive.
	const int chunkWidth = (int)(tileWidth / (float)CHUNKSIZE);
	const int chunkHeight = (int)(tileHeight / (float)CHUNKSIZE);
	if (chunkWidth == 0 || chunkHeight == 0) {
		p_finished = true;
		return std::nullopt;
	};

	if (m_setFetchCounterFlag) {
		m_setFetchCounterFlag = false;
		m_fetchCounter = 0;
	}
	if (!p_finished) {
		int curX = m_fetchCounter % chunkWidth;
		int curY = m_fetchCounter / chunkWidth;
		if (curY <= chunkHeight) {
			m_fetchCounter++;
			bool tmp;
			const int worldX = curX + (int)std::floorf(x / (float)CHUNKSIZE);
			const int worldY = curY + (int)std::floorf(y / (float)CHUNKSIZE);
			return getChunkPtr(ChunkPos(worldX, worldY), tmp);
		}
	}

	p_finished = true;
	m_setFetchCounterFlag = true;
	return std::nullopt;
}
glm::ivec4 frameToChunkCoords(glm::vec4 p_frame) {
	int x1 = utils::gridFloor(p_frame.x, CHUNKSIZE);
	int y1 = utils::gridFloor(p_frame.y, CHUNKSIZE) + 1;
	int x2 = utils::gridFloor(p_frame.z, CHUNKSIZE) + 1;
	int y2 = utils::gridFloor(p_frame.w, CHUNKSIZE) + 2;
	return glm::ivec4(x1, y1, x2, y2);
}
void ChunkManager::updateDrawList(glm::vec4 p_frame) {
	// TODO:: Make this smarter such that it doesn't need to rebuild the list completely when the frame moves
	static glm::ivec4 prev = glm::ivec4(0);
	glm::ivec4 next = frameToChunkCoords(p_frame);
	if (prev == next) return;
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
	//LOG(next.x << " " << next.y << " " << next.z << " " << next.w);
	prev = next;
}
int ChunkManager::drawVisible(DrawSurface& p_target, DrawStates& p_states, Shader& p_tileShader) {
	int count = 0;
	for (auto chunk : m_drawList) {
		if (chunk->isEmpty) continue;
		if (!chunk->meshIsCurrent) chunk->generateVBO(*this);
		p_tileShader.setVec2Uniform("worldPos", glm::vec2(chunk->worldPos.x, chunk->worldPos.y));
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

bool ChunkManager::removeChunk(ChunkPos p_chunkPos)
{
	return (bool)m_chunkMap.erase(p_chunkPos);
}

void ChunkManager::removeChunks()
{
	m_chunkMap.clear();
}
// draws every chunk in the manager, innefficent
//void ChunkManager::draw(DrawSurface& p_target, DrawStates& p_drawStates) {
//	if (m_chunkList.size() == 0) return;
//	for (size_t i = 0; i < m_chunkList.size(); i++) {
//		if (m_chunkList[i].isEmpty) continue;
//		if (!m_chunkList[i].meshIsCurrent) m_chunkList[i].generateVBO();
//		m_chunkList[i].draw(p_target, p_drawStates);
//	}
//}

