#include "World.hpp"
#include "GameRenderer.hpp"

bool World::genChunk(glm::ivec2 p_worldPos)
{
	bool chunkExists;
	World::getChunk(p_worldPos, chunkExists);
	if (!chunkExists) {
		WorldChunk chunk = WorldChunk(p_worldPos, 0);
		chunk.worldGenerate(p_worldPos);
		chunkMap.insert(std::make_pair(p_worldPos, chunk));
		return true;
	}
	return false;
}
bool World::genChunk(int p_chunkX, int p_chunkY)
{
	bool chunkExists;
	World::getChunk(glm::ivec2(p_chunkX, p_chunkY), chunkExists);
	if (!chunkExists) {
		WorldChunk chunk = WorldChunk(glm::ivec2(p_chunkX, p_chunkY), 0);
		chunk.worldGenerate(glm::ivec2(p_chunkX, p_chunkY));
		chunkMap.insert(std::make_pair(glm::ivec2(p_chunkX, p_chunkY), chunk));
		return true;
	}
	return false;
}
void World::enqueueGen(glm::ivec2 p_chunkPos)
{
	//std::cout << loadQueue.size() << std::endl;
	if (!World::chunkExistsAt(p_chunkPos)) {
		loadQueue.push(p_chunkPos);
	};
}

void World::genFromQueue()
{
	glm::ivec2 chunkPos;
	if (!loadQueue.empty()) {
		chunkPos = loadQueue.back();
		loadQueue.pop();
	}
	else { return; }
	genChunk(chunkPos);
}

bool World::autoGen(Camera& p_cam) {

	for (int i = (int)(p_cam.getFrame().y / (float)CHUNKSIZE) - 2;
		i < (int)((p_cam.getFrame().w) / (float)CHUNKSIZE) + 2;
		i++) {
		for (int j =
			(int)(p_cam.getFrame().x / (float)CHUNKSIZE) - 2;
			j < (int)((p_cam.getFrame().z) / (float)CHUNKSIZE) + 2;
			j++) {
			if ((i > 0) && (i < 12) && (j > -20) && (j < 20)) {
				World::enqueueGen(glm::ivec2(j, i));
			}
		}
	}
	return false;
}
void World::logSize() {
	std::cout << chunkMap.size();
}
void World::logChunks() {
	std::map<wc::ivec2, WorldChunk>::iterator it = chunkMap.begin();
	while (it != chunkMap.end()) {
		std::cout << it->first.x << " " << it->first.y << " : " << it->second.tileMesh.VAO << std::endl;
		it++;
	}
}
WorldChunk& World::getChunk(glm::ivec2 p_worldPos, bool& success) { // actually gets the chunk
	if (chunkMap.size() == 0) { success = false; WorldChunk nullChunk; return nullChunk; } // don't know of a better thing to return yet, nullChunk is dealloced apon scope end
	std::map<wc::ivec2, WorldChunk>::iterator it = chunkMap.find(p_worldPos);
	if (it != chunkMap.end()) {
		success = true;
		return it->second;
	}
	success = false;
}
bool World::chunkExistsAt(glm::ivec2 p_worldPos) { // tells you if a chunk exists
	std::map<wc::ivec2, WorldChunk>::iterator it = chunkMap.find(p_worldPos);
	if (it != chunkMap.end()) {
		return true;
	}
	return false;
}

bool World::removeChunk(glm::ivec2 p_worldPos)
{

	if (chunkMap.erase(p_worldPos) > 0) {
		return true;
	};
	return false;
}
void World::removeChunk()
{
	auto eraseIter = chunkMap.begin();
	if (eraseIter == chunkMap.end()) return;
	eraseIter->second.remove();
	std::advance(eraseIter, 1);
	chunkMap.erase(chunkMap.begin(), eraseIter);
}

void World::removeChunks()
{
	auto it = chunkMap.begin();
	while (it != chunkMap.end()) {
		it->second.remove();
		it++;
	}
	chunkMap.clear();
}
void World::drawWorld(GameRenderer& renderer, GameWindow& gw) {
	auto it = chunkMap.begin();
	while (it != chunkMap.end()) { // go through all the chunks in the world
		glm::vec2 chunkGlobalPos = it->second.worldPos * CHUNKSIZE;
		if (
			(chunkGlobalPos.x > renderer.cam.getFrame().x - 1 * CHUNKSIZE) &&
			(chunkGlobalPos.x < renderer.cam.getFrame().z + 1 * CHUNKSIZE) &&
			(chunkGlobalPos.y > renderer.cam.getFrame().y - 1 * CHUNKSIZE) &&
			(chunkGlobalPos.y < renderer.cam.getFrame().w + 1 * CHUNKSIZE)
			) {
			renderer.drawChunk(it->second);
		}
		++it;
	};
}
//World::~World() {
//	std::map<wc::ivec2, WorldChunk>::iterator it = chunkMap.begin();
//	while (it != chunkMap.end()) {
//		std::cout << "Attempting to delete chunk at " << it->second.worldPos.x << ", " << it->second.worldPos.y << std::endl;
//		it->second.destroy();
//		it++;
//	}
//}

