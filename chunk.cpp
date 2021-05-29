#include "Chunk.h"

WorldChunk::WorldChunk(Vector2i p_worldPos, int p_worldID) :worldPos(p_worldPos), worldID(p_worldID) {
	for (int y = 0; y < 128; y++) {
		tiles[y] = new Tile[128];
	}
}
void WorldChunk::cleanUp() {
	for (int y = 1; y < 128; y++) {
		std::cout << /*typeid(tiles[y]).name()*/"THIS LINE OF CODE WAS EXECUTED" << std::endl;
		delete tiles[y];
	}
	delete tiles;
}
void WorldChunk::fillRandom() {
	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			std::cout << tiles[y][x].getChunkPos().x << std::endl;
		}
	}
	return;
}

void WorldChunk::setChunkTile(Vector2i p_chunkCoordinates) {
	return;
}