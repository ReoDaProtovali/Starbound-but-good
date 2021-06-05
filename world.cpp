#include "World.hpp"

World::World() {
	chunks.push_back(WorldChunk());
	chunks.push_back(WorldChunk(Vector2i(1,0)));
}

WorldChunk& World::getChunk(Vector2i p_worldPos) {
	for (WorldChunk& c : chunks) {
		if ((c.worldPos.x == p_worldPos.x) && (c.worldPos.y == p_worldPos.y)) {
			//std::cout << "Found chunk: " << p_worldPos.x << " " << p_worldPos.y << std::endl;
			return c;
		}
	}
}
World::~World() {
	for (WorldChunk& c : chunks) {
		std::cout << "Attempting to delete chunk at " << c.worldPos.x << ", " << c.worldPos.y << std::endl;
		c.cleanUp();
	}
}