#pragma once

#include "util/utils.hpp"
#include "GameConstants.hpp"
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/TransformObject.hpp"
#include "ResourceManager.hpp"


#include "Tile.hpp"
#include "TileVert.hpp"
#include <util/ext/glm/vec2.hpp>
#include <util/ext/glm/vec3.hpp>
#include <util/ext/glm/vec4.hpp>

#include <vector>
#include "util/Array3D.hpp"
#include "WorldGenNoisemap.hpp"
#include "util/SharedQueue.hpp"

#include <atomic>
#include <box2d.h>

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
		return ((size_t)p_key.y << 32) ^ (size_t)p_key.x;
	}
	int32_t x;
	int32_t y;
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

	WorldChunk& operator=(WorldChunk&& other) noexcept;


	WorldChunk(WorldChunk&& other) noexcept;

	void fillRandom();
	void setChunkTile(glm::ivec3 p_chunkCoordinates, uint32_t p_tileID); // unimplemented
	uint8_t getTileAdjacent(int p_x, int p_y, int p_z);
	void setTiles(Array3D<Tile>&& p_tiles);
	Tile& getChunkTile(size_t p_x, size_t p_y, size_t p_z);
	// conditionally returns either an internal tile or a tile from a neigboring chunk based on the given position.
	std::optional<Tile*> getNeigboringChunkTile(int p_chunkX, int p_chunkY, int p_chunkZ, ChunkManager& p_chnks);
	std::optional<WorldChunk*> getIntrudedChunk(int p_localTileX, int p_localTileY, int localTileZ, ChunkManager& p_chnks);
	Tile& operator()(size_t p_x, size_t p_y, size_t p_depth);
	void generateVBO(ChunkManager& p_chnks);
	void genSingleTileVBO(int p_tileX, int p_tileY, int p_tileZ, ChunkManager& p_chnks);

	void genCollider(b2World& p_world, ChunkManager& tmp);

	void pushVBO();
	void subSingleTileVBOS();
	uint32_t getVBOSize();

	void draw(DrawSurface& p_target, DrawStates& p_drawStates);

	void flip();

	void remove();

	ChunkPos worldPos;
	int worldID;

	std::atomic<bool> vboIsPushed{ false };
	std::atomic<bool> drawable{ false };
	std::atomic<bool> invalid{ true };
	std::atomic<bool> isEmpty{ true };
	std::atomic<bool> colliderValid{ false };
	std::atomic<bool> feedbackMeshReady = false;
	Mesh<TileVert> tileMesh{NO_VAO_INIT};
	//struct PostGSVert {
	//	glm::vec2 TexCoord;
	//	float zLevel;
	//	glm::vec4 pos;
	//};
	//Mesh<PostGSVert> feedbackMesh{ (MeshBehavior)(FEEDBACK_MESH | NO_VAO_INIT) };

	SharedQueue<glm::ivec3> tilesToSub;


	b2World* associatedWorld = nullptr; // so it can delete its own collider
	std::mutex m_vboMutex;

private:
	std::mutex m_colliderGenMutex;
	Array3D<Tile> m_tiles;

	b2Body* m_collisionBody = nullptr;

};
