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
bool World::autoGen(Camera& p_cam) {

	for (int i = (int)(p_cam.getFrame().y / (float)CHUNKSIZE) - 2;
		i < (int)((p_cam.getFrame().w) / (float)CHUNKSIZE) + 2;
		i++) {
		for (int j =
			(int)(p_cam.getFrame().x / (float)CHUNKSIZE) - 2;
			j < (int)((p_cam.getFrame().z) / (float)CHUNKSIZE) + 2;
			j++) {
			if ((i > 0) && (i < 12) && (j > -20) && (j < 20)) {
				World::genChunk(j, i);
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
		std::cout << it->first.x << " " << it->first.y << " : " << it->second.VAO << std::endl;
		it++;
	}
}
WorldChunk& World::getChunk(glm::ivec2 p_worldPos, bool& success) {
	WorldChunk nullChunk;
	if (chunkMap.size() == 0) { success = false; return nullChunk; }
	std::map<wc::ivec2, WorldChunk>::iterator it = chunkMap.find(p_worldPos);
	if (it != chunkMap.end()) {
		success = true;
		return it->second;
	}
	success = false;
}
WorldChunk& World::getChunk(glm::ivec2 p_worldPos) {

	std::map<wc::ivec2, WorldChunk>::iterator it = chunkMap.find(p_worldPos);
	if (it != chunkMap.end()) {
		return it->second;
	}
}
void World::drawWorld(GameRenderer& renderer, GameWindow& gw) {
	std::map<wc::ivec2, WorldChunk>::iterator it = chunkMap.begin();
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
//		it->second.cleanUp();
//		it++;
//	}
//}

