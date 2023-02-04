#pragma once

#include "util/utils.hpp"
#include "GameConstants.hpp"
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/TransformObject.hpp"
#include "ResourceManager.hpp"


#include "Tile.hpp"
#include "TileVert.h"
#include <util/ext/glm/vec2.hpp>
#include <util/ext/glm/vec3.hpp>
#include <util/ext/glm/vec4.hpp>

#include <vector>
#include "util/Array3D.hpp"
#include "WorldGenNoisemap.hpp"

struct ChunkPos {
	ChunkPos() { x = -1; y = -1; }
	ChunkPos(int p_x, int p_y) : x(p_x), y(p_y) {};
	friend bool operator<(const ChunkPos& pos1, const ChunkPos& pos2) {
		if (pos1.y < pos2.y) return true;
		if (pos1.y > pos2.y) return false;
		if (pos1.x < pos2.x) return true;
		else return false;
	}
	friend bool operator==(const ChunkPos& pos1, const ChunkPos& pos2) {
		if (pos1.x == pos2.x && pos1.y == pos2.y) return true;
		return false;
	}
	size_t operator()(const ChunkPos& p_key) const {
		return (p_key.y << 16) ^ p_key.x;
	}
	int x;
	int y;
};

class ChunkManager;
struct WorldChunk : public TransformObject
{
	// Invalid constructor
	WorldChunk(void) : 
		worldID(-1),
		worldPos(ChunkPos(9999, 9999)),
		invalid(true) {};

	WorldChunk(ChunkPos p_chunkPos, int p_worldID);

	WorldChunk(const WorldChunk& other) noexcept;

	WorldChunk& operator=(const WorldChunk& other);

	WorldChunk(WorldChunk&& other) noexcept;

	void fillRandom();
	void worldGenerate(WorldGenNoisemap& noiseGen);
	void setChunkTile(glm::ivec3 p_chunkCoordinates, uint32_t p_tileID); // unimplemented
	Tile& getChunkTile(int p_x, int p_y, int p_z);
	// conditionally returns either an internal tile or a tile from a neigboring chunk based on the given position.
	std::optional<Tile*> getNeigboringChunkTile(int p_chunkX, int p_chunkY, int p_chunkZ, ChunkManager& p_chnks);

	Tile* getTiles();
	void generateVBO(ChunkManager& p_chnks);

	void pushVBO();
	uint32_t getVBOSize();

	void flip();

	void remove();

	ChunkPos worldPos;
	int worldID;

	bool vertsAreCurrent = false;
	bool vboIsPushed = false;
	bool invalid = true;
	bool isEmpty = true;
	Mesh<TileVert> tileMesh{NO_VAO_INIT};

	void draw(DrawSurface& p_target, DrawStates& p_drawStates);

private:
	Array3D<Tile> m_tiles;

};
