#include "ChunkManager.hpp"
#include <cstdio>

bool ChunkManager::genChunk(ChunkPos p_chunkPos)
{
	if (chunkExistsAt(p_chunkPos)) return false;

	// add it to the back of the translation map
	m_indices.push_back(ChunkPos(p_chunkPos));
	m_chunkList.emplace_back(p_chunkPos, 0);// world ID is hardcoded for now. Will most def be a different system later.
	m_chunkList.back().worldGenerate(); // prevent copying by getting it from the end. generation queue will be separate later.
	return true;
}
bool ChunkManager::genChunk(int p_chunkX, int p_chunkY)
{
	if (chunkExistsAt(ChunkPos(p_chunkX, p_chunkY))) return false;

	// add it to the back of the translation map
	m_indices.push_back(ChunkPos(p_chunkX, p_chunkY));
	m_chunkList.emplace_back(ChunkPos(p_chunkX, p_chunkY), 0);// world ID is hardcoded for now. Will most def be a different system later.
	m_chunkList.back().worldGenerate(); // prevent copying by getting it from the end. generation queue will be separate later.
	return true;
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
			if ((i > 0) && (i < 24) && (j > -40) && (j < 40)) {
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
	printf("Chunk count: %zu\n", m_chunkList.size());
}
int ChunkManager::getEmptyChunkCount()
{
	int count = 0;
	for (size_t i = 0; i < m_chunkList.size(); i++) {
		if (m_chunkList[i].isEmpty) count++;
	}
	return count;
}
void ChunkManager::logChunks() {

	for (size_t i = 0; i < m_chunkList.size(); i++) {
		printf("There is a chunk at position %i %i with index %zu\n", m_chunkList[i].worldPos.x, m_chunkList[i].worldPos.y, i);
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
	size_t index = findChunkIndex(p_chunkPos, p_success);

	if (!p_success) return std::nullopt;
	return std::optional<WorldChunk*>(&m_chunkList[index]);
}
size_t ChunkManager::findChunkIndex(ChunkPos p_chunkPos, bool& p_success) {
	for (size_t i = 0; i < m_indices.size(); i++) {
		if (m_indices[i] == p_chunkPos) {
			p_success = true;
			return i;
		}
	}
	p_success = false;
	return 999;
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
bool ChunkManager::chunkExistsAt(ChunkPos p_chunkPos) { // tells you if a chunk exists
	// linear search, could be hashed but meh
	for (size_t i = 0; i < m_indices.size(); i++) {
		if (m_indices[i] == p_chunkPos) {
			return true;
		}
	}
	return false;
}

bool ChunkManager::removeChunk(ChunkPos p_chunkPos)
{
	bool success;
	size_t removeIndex = findChunkIndex(p_chunkPos, success);
	if (!success) return false;
	auto eraseChunkIter = m_chunkList.begin() + removeIndex;
	auto eraseIndexIter = m_indices.begin() + removeIndex;
	m_chunkList.erase(eraseChunkIter);
	m_indices.erase(eraseIndexIter);
	return true;
}

void ChunkManager::removeChunks()
{
	//for (auto& w : chunkList) {
	//	w->remove();
	//}
	m_chunkList.clear();
	m_indices.clear();
}
// draws every chunk in the manager, innefficent
void ChunkManager::draw(DrawSurface& p_target, DrawStates& p_drawStates) {
	if (m_chunkList.size() == 0) return;
	for (size_t i = 0; i < m_chunkList.size(); i++) {
		if (m_chunkList[i].isEmpty) continue;
		if (!m_chunkList[i].meshIsCurrent) m_chunkList[i].generateVBO();
		m_chunkList[i].draw(p_target, p_drawStates);
	}
}

