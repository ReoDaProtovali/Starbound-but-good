#include "ChunkManager.hpp"
#include <cstdio>

bool ChunkManager::genChunk(ChunkPos p_chunkPos)
{
	if (chunkExistsAt(p_chunkPos)) return false;

	// add it to the back of the translation map
	indices.push_back(ChunkPos(p_chunkPos));
	chunkList.push_back(std::make_shared<WorldChunk>(p_chunkPos, 0));// world ID is hardcoded for now. Will most def be a different system later.
	chunkList.back()->worldGenerate(); // prevent copying by getting it from the end. generation queue will be separate later.
	return true;
}
bool ChunkManager::genChunk(int p_chunkX, int p_chunkY)
{
	if (chunkExistsAt(ChunkPos(p_chunkX, p_chunkY))) return false;

	// add it to the back of the translation map
	indices.push_back(ChunkPos(p_chunkX, p_chunkY));
	chunkList.push_back(std::make_shared<WorldChunk>(ChunkPos(p_chunkX, p_chunkY), 0));// world ID is hardcoded for now. Will most def be a different system later.
	chunkList.back()->worldGenerate(); // prevent copying by getting it from the end. generation queue will be separate later.
	return true;
}
void ChunkManager::enqueueGen(ChunkPos p_chunkPos)
{
	//std::cout << chunkExistsAt(ChunkPos(0, 0)) << std::endl;
	//std::cout << loadQueue.size() << std::endl;
	if (!ChunkManager::chunkExistsAt(p_chunkPos)) {
		loadQueue.push(p_chunkPos);
	};
}

bool ChunkManager::genFromQueue()
{

	ChunkPos chunkPos;
	if (loadQueue.empty()) return false;

	//std::cout << loadQueue.size() << std::endl;
	chunkPos = loadQueue.front();
	loadQueue.pop();
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
	printf("Chunk count: %zu\n", chunkList.size());
}
int ChunkManager::getEmptyChunkCount()
{
	int count = 0;
	for (size_t i = 0; i < chunkList.size(); i++) {
		if (chunkList[i]->isEmpty) count++;
	}
	return count;
}
void ChunkManager::logChunks() {

	for (size_t i = 0; i < chunkList.size(); i++) {
		printf("There is a chunk at position %i %i with index %zu\n", chunkList[i]->worldPos.x, chunkList[i]->worldPos.y, i);
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
std::weak_ptr<WorldChunk> ChunkManager::getChunkPtr(ChunkPos p_chunkPos, bool& p_success)
{
	bool found;
	size_t index = findChunkIndex(p_chunkPos, found);
	p_success = found;
	// still kinda scuffed but it gotta return something
	if (!found) return nullChunk;
	return chunkList[index];
}
size_t ChunkManager::findChunkIndex(ChunkPos p_chunkPos, bool& p_success) {
	for (size_t i = 0; i < indices.size(); i++) {
		if (indices[i] == p_chunkPos) {
			p_success = true;
			return i;
		}
	}
	p_success = false;
	return 999;
}
std::weak_ptr<WorldChunk> ChunkManager::fetchFromFrame(glm::vec4 p_viewFrame, bool& p_finished)
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
		return nullChunk;
	};

	if (setFetchCounterFlag) {
		setFetchCounterFlag = false;
		fetchCounter = 0;
	}
	if (!p_finished) {
		int curX = fetchCounter % chunkWidth;
		int curY = fetchCounter / chunkWidth;
		if (curY <= chunkHeight) {
			fetchCounter++;
			bool tmp;
			const int worldX = curX + (int)std::floorf(x / (float)CHUNKSIZE);
			const int worldY = curY + (int)std::floorf(y / (float)CHUNKSIZE);
			return getChunkPtr(ChunkPos(worldX, worldY), tmp);
		}
	}

	p_finished = true;
	setFetchCounterFlag = true;
	return nullChunk;
}
bool ChunkManager::chunkExistsAt(ChunkPos p_chunkPos) { // tells you if a chunk exists
	for (size_t i = 0; i < indices.size(); i++) {
		if (indices[i] == p_chunkPos) {
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
	auto eraseChunkIter = chunkList.begin() + removeIndex;
	auto eraseIndexIter = indices.begin() + removeIndex;
	chunkList.erase(eraseChunkIter);
	indices.erase(eraseIndexIter);
	return true;
}

void ChunkManager::removeChunks()
{
	//for (auto& w : chunkList) {
	//	w->remove();
	//}
	chunkList.clear();
	indices.clear();
}
// draws every chunk in the manager, innefficent
void ChunkManager::draw(DrawSurface& p_target, DrawStates& p_drawStates) {
	if (chunkList.size() == 0) return;
	for (size_t i = 0; i < chunkList.size(); i++) {
		if (chunkList[i]->isEmpty) continue;
		if (!chunkList[i]->meshIsCurrent) chunkList[i]->generateVBO();
		chunkList[i]->draw(p_target, p_drawStates);
	}
}

