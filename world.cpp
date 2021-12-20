#include "World.hpp"

bool World::genChunk(glm::ivec2 p_worldPos)
{
	bool chunkExists;
	World::getChunk(p_worldPos, chunkExists);
	if (!chunkExists) {
		WorldChunk chunk = WorldChunk(p_worldPos, 0);
		chunk.fillRandom();
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
		chunk.fillRandom();
		chunkMap.insert(std::make_pair(glm::ivec2(p_chunkX, p_chunkY), chunk));
		return true;
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
	if (chunkMap.size() == 0) { success = false; return nullChunk;  }
	std::map<wc::ivec2, WorldChunk>::iterator it = chunkMap.find(p_worldPos);
	if (it != chunkMap.end()) {
		success = true;
		return it->second;
	}
	success = false;
	return nullChunk;

}
WorldChunk& World::getChunk(glm::ivec2 p_worldPos) {

	std::map<wc::ivec2, WorldChunk>::iterator it = chunkMap.find(p_worldPos);
	if (it != chunkMap.end()) {
		return it->second;
	}
	WorldChunk nullChunk;
	return nullChunk;
}
World::~World() {
	std::map<wc::ivec2, WorldChunk>::iterator it = chunkMap.begin();
	while (it != chunkMap.end()) {
		//std::cout << "Attempting to delete chunk at " << it->second.worldPos.x << ", " << it->second.worldPos.y << std::endl;
		it->second.cleanUp();
		it++;
	}
}

