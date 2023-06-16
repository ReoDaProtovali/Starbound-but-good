#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include <util/ext/glm/glm.hpp>
#include <stdexcept>
#include "GameConstants.hpp"
#include <unordered_set>
#include "util/StaticArray2D.hpp"
#include "util/StaticArray3D.hpp"
WorldChunk::WorldChunk(ChunkPos p_chunkPos, int p_worldID) :
	worldPos(p_chunkPos),
	worldID(p_worldID),
	invalid(false),
	m_tiles(CHUNKSIZE, CHUNKSIZE, CHUNKDEPTH)
{
	setPosition(glm::vec3((float)p_chunkPos.x * CHUNKSIZE * 1.2, (float)p_chunkPos.y * CHUNKSIZE, 0.f));
	calculateTransform();
	tileMesh.addUintAttrib(1); // xyzID, one uint
	tileMesh.addUintAttrib(1); // adjacencies
	tileMesh.addUintAttrib(1); // variation count i guess
	feedbackMesh.addFloatAttrib(2);
	feedbackMesh.addFloatAttrib(1);
	feedbackMesh.addFloatAttrib(4);
	feedbackMesh.setStreamType(GL_DYNAMIC_DRAW);
}

WorldChunk::WorldChunk(const WorldChunk& other) noexcept :
	tileMesh(other.tileMesh),
	feedbackMesh(other.feedbackMesh),
	m_tiles(other.m_tiles),
	worldID(other.worldID),
	worldPos(other.worldPos),
	invalid(other.invalid.load()),
	vboIsPushed(other.vboIsPushed.load()),
	isEmpty(other.isEmpty.load())
{
	// Don't forget the base class
	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE, 0.f));
	calculateTransform();
}

WorldChunk& WorldChunk::operator=(const WorldChunk& other)
{

	tileMesh = other.tileMesh;
	feedbackMesh = other.feedbackMesh;
	m_tiles = other.m_tiles;
	worldID = other.worldID;
	worldPos = other.worldPos;
	vboIsPushed = other.vboIsPushed.load();
	invalid = other.invalid.load();
	isEmpty = other.isEmpty.load();

	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE, 0.f));
	calculateTransform();

	return *this;
}
WorldChunk& WorldChunk::operator=(WorldChunk&& other) noexcept
{
	tileMesh = std::move(other.tileMesh);
	feedbackMesh = std::move(other.feedbackMesh);
	m_tiles = std::move(other.m_tiles);
	worldID = other.worldID;
	worldPos = other.worldPos;
	vboIsPushed = other.vboIsPushed.load();
	invalid = other.invalid.load();
	isEmpty = other.isEmpty.load();

	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE, 0.f));
	calculateTransform();

	return *this;
}
;

WorldChunk::WorldChunk(WorldChunk&& other) noexcept :
	tileMesh(std::move(other.tileMesh)),
	feedbackMesh(std::move(other.feedbackMesh)),
	m_tiles(std::move(other.m_tiles)),
	worldID(other.worldID),
	worldPos(other.worldPos),
	vboIsPushed(other.vboIsPushed.load()),
	invalid(other.invalid.load()),
	isEmpty(other.isEmpty.load())
{
	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE, 0.f));
	calculateTransform();
}

void WorldChunk::fillRandom() {

	for (int z = 0; z < CHUNKDEPTH; z++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int x = 0; x < CHUNKSIZE; x++) {
				if (rand() % 10 > 5) {
					m_tiles(x, y, z) = Tile(glm::ivec2(x, y), 1);
				}
				else {
					m_tiles(x, y, z) = Tile(glm::ivec2(x, y), 2);
				}
			}
		}
	}
	isEmpty = false;
	colliderValid = false;
}
void WorldChunk::setTiles(Array3D<Tile>&& p_tiles) {
	m_tiles = p_tiles;
	colliderValid = false;
	feedbackMeshReady = false;
}
Tile& WorldChunk::operator()(size_t p_x, size_t p_y, size_t p_depth)
{
	return m_tiles(p_x, p_y, p_depth);
}
void WorldChunk::generateVBO(ChunkManager& p_chnks) {
	std::unique_lock<std::mutex> lock(m_vboMutex);
	if (isEmpty) return;
	tileMesh.clean();
	tileMesh.reserve((size_t)CHUNKSIZE * CHUNKSIZE * CHUNKDEPTH * sizeof(TileVert)); // reserve a chunks worth of data idk
	ResourceManager& res = ResourceManager::Get();

	for (int z = 0; z < CHUNKDEPTH; z++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int x = 0; x < CHUNKSIZE; x++) {
				uint32_t tID = m_tiles(x, y, z).m_tileID;

				TileVert v = TileVert(
					x, y, z, // Position attributes
					tID // numerical ID
				);

				if (m_tiles(x, y, z).m_tileID == 0) {
					tileMesh.pushVertex(v);
					continue;
				};
				// it's teeechnically possible to convert the image id into the cache id by subtracting one
				TileInfo& tInfo = res.getTileInfo(tID - 1);
				v.variationCount = tInfo.variationCount;

				//// Remove non-visible tiles
				//for (int layer = CHUNKDEPTH - 1; layer > z; layer--) {
				//	uint8_t tileCount = 0;
				//	for (int i = -1; i <= 1; i++) {
				//		for (int j = -1; j <= 1; j++) {
				//			auto opt = getNeigboringChunkTile(x + j, y + i, layer, p_chnks);
				//			if (opt.has_value()) {
				//				if (!opt.value()) continue;
				//				if (opt.value()->m_tileID == 0) continue;
				//				if (res.getTileInfo(opt.value()->m_tileID - 1).transparent) continue;
				//				if (opt.value()->m_tileID != 0) tileCount++;
				//			}
				//		}
				//	}

					//if (tileCount == 9) {
						//goto skip;
					//}
				//}

				// Set tile adjacencies
				for (int i = 0; i < 9; i++) {
					if (i == 4) continue;

					auto opt = getNeigboringChunkTile(x + (i % 3) - 1, y + (i / 3) - 1, z, p_chnks);

					auto centerOpt = getNeigboringChunkTile(x, y, z, p_chnks);

					if (opt.has_value()) {
						if (opt.value()->m_tileID != 0) continue;
					}

					v.setAdjacent(128u >> (i > 4 ? i - 1 : i));

				}
				tileMesh.pushVertex(v);
			//skip:
			//	{}
			}
		}
	}
	vboIsPushed = false;

}

void WorldChunk::genSingleTileVBO(int p_tileX, int p_tileY, int p_tileZ, ChunkManager& p_chnks)
{
	ResourceManager& res = ResourceManager::Get();
	// this is mostly dumb stuff i have to do for the edge case
	auto tileOpt = getNeigboringChunkTile(p_tileX, p_tileY, p_tileZ, p_chnks);
	if (!tileOpt.has_value()) return;
	Tile* thisTile = tileOpt.value();
	int x = thisTile->getChunkPos().x;
	int y = thisTile->getChunkPos().y;
	int z = p_tileZ;

	WorldChunk* currentChunk = this;
	// this will be the case if the tile is outside of this chunk
	if (x != p_tileX || y != p_tileY) {
		if (getIntrudedChunk(p_tileX, p_tileY, z, p_chnks).has_value()) {
			currentChunk = getIntrudedChunk(p_tileX, p_tileY, z, p_chnks).value();
		}
	}
	std::unique_lock<std::mutex> lock(currentChunk->m_vboMutex);
	if (currentChunk->isEmpty) return;
	if (currentChunk->invalid) return;
	if (currentChunk->tileMesh.getTotalVBOSize() == 0) return;
	uint32_t tID = thisTile->m_tileID;

	TileVert v = TileVert(
		x, y, z, // Position attributes
		tID // numerical ID
	);


	// it's teeechnically possible to convert the image id into the cache id by subtracting one
	if (tID != 0) {
		TileInfo& tInfo = res.getTileInfo(tID - 1);
		v.variationCount = tInfo.variationCount;
	}
	// Set tile adjacencies
	for (int i = 0; i < 9; i++) {
		if (i == 4) continue;

		auto opt = currentChunk->getNeigboringChunkTile(x + (i % 3) - 1, y + (i / 3) - 1, z, p_chnks);

		auto centerOpt = currentChunk->getNeigboringChunkTile(x, y, z, p_chnks);

		if (opt.has_value()) {
			if (opt.value()->m_tileID != 0) continue;
		}

		v.setAdjacent(128u >> (i > 4 ? i - 1 : i));

	}
	// only doing this because too lazy to do 3d array access math lol
	StaticArray3D<TileVert> tmpArr(CHUNKSIZE, CHUNKSIZE, CHUNKDEPTH);
	tmpArr.setData(currentChunk->tileMesh.getVerts().data());
	tmpArr(x, y, z) = v;
	currentChunk->tilesToSub.push(glm::ivec3(x, y, z));
	// so it doesn't self delete
	tmpArr.setData(nullptr);
}

inline b2Vec2 localToColliderP(int x, int y)
{
	return b2Vec2((float)x * TILES_TO_METERS - float(CHUNKSIZE) * TILES_TO_METERS * 0.5f, -(float)y * TILES_TO_METERS + float(CHUNKSIZE) * TILES_TO_METERS * 0.5f);
}

// 0 is empty, 1 is solid
uint8_t WorldChunk::getTileAdjacent(int p_x, int p_y, int p_z) {
	uint8_t result = 0;
	for (int i = 0; i < 9; i++) {
		if (i == 4) continue;
		int x = p_x + (i % 3) - 1;
		int y = p_y + (i / 3) - 1;
		if (!m_tiles.bounded(x, y, p_z)) continue;
		if (m_tiles(x, y, p_z).m_tileID == 0) continue;
		result |= 128u >> (i > 4 ? i - 1 : i);
	}
	return result;
}
void WorldChunk::genCollider(b2World& p_world, ChunkManager& tmp)
{
	std::unique_lock<std::mutex> lock(m_colliderGenMutex);
	associatedWorld = &p_world;
	//if (colliderValid) return;
	if (isEmpty) return;
	if (m_collisionBody) p_world.DestroyBody(m_collisionBody);
	float worldMetersX = float(worldPos.x * CHUNKSIZE) * TILES_TO_METERS;
	float worldMetersY = float(worldPos.y * CHUNKSIZE) * TILES_TO_METERS;
	constexpr float chunkSizeMeters = float(CHUNKSIZE) * TILES_TO_METERS;


	b2BodyDef bodyDef;

	bodyDef.position.Set(worldMetersX + chunkSizeMeters / 2.f, worldMetersY - chunkSizeMeters / 2.f);
	bodyDef.type = b2_staticBody;
	m_collisionBody = p_world.CreateBody(&bodyDef);


	std::vector<b2Vec2> verts = {};

	verts.reserve(1024);

	StaticArray2D<uint8_t> visitedEdges;
	visitedEdges.resize(CHUNKSIZE, CHUNKSIZE);
	// visitedTiles owns the data now, it will be freed
	visitedEdges.setData((uint8_t*)calloc((size_t)CHUNKSIZE * CHUNKSIZE, sizeof(uint8_t)));

	for (int y = 0; y < CHUNKSIZE; y++) {
		for (int x = 0; x < CHUNKSIZE; x++) {
			if (m_tiles(x, y, 3).m_tileID == 0) continue;
			uint8_t adj = getTileAdjacent(x, y, 3);
			if ((adj & 0b01011010) == 0b01011010) continue;
			if (!(adj & 0b01000000)) { // we'll just start loops from wherever there's a free top tile
				int edgeTileX = x;
				int edgeTileY = y;
				// tells you which side of the tile the current edge is on
				// 0 is top
				// 1 is left
				// 2 is bottom
				// 3 is right
				int currentSide = 0;
				// don't start a new loop if one is already here
				if (visitedEdges(x, y) & 0b1000) continue;
				verts.push_back(localToColliderP(edgeTileX + 1, edgeTileY));
				verts.push_back(localToColliderP(edgeTileX, edgeTileY));

				while (true) {
					adj = getTileAdjacent(edgeTileX, edgeTileY, 3);
					switch (currentSide) {
						// top edge
						// . _ .
						// . # .
						// . . .
					case 0:
						if (visitedEdges(edgeTileX, edgeTileY) & 0b1000) goto out;
						// top left
						// #|_ .
						// . # .
						// . . .
						if (adj & 0b10000000) {
							verts.push_back(localToColliderP(edgeTileX, edgeTileY - 1));
							visitedEdges(edgeTileX, edgeTileY) |= 0b1000;
							//m_tiles(edgeTileX, edgeTileY, 3).m_tileID = 3;
							edgeTileX--;
							edgeTileY--;
							currentSide = 3;
							break;
						}
						// left
						// _ _ .
						// # # .
						// . . .
						if (adj & 0b00010000) {
							verts.push_back(localToColliderP(edgeTileX - 1, edgeTileY));
							visitedEdges(edgeTileX, edgeTileY) |= 0b1000;
							//m_tiles(edgeTileX, edgeTileY, 3).m_tileID = 3;
							edgeTileX--;
							currentSide = 0;
							break;
						}
						// no tile in direction of traversal
						// . _ .
						// .|# .
						// . . .
						verts.push_back(localToColliderP(edgeTileX, edgeTileY + 1));
						visitedEdges(edgeTileX, edgeTileY) |= 0b1000;
						//m_tiles(edgeTileX, edgeTileY, 3).m_tileID = 3;
						currentSide = 1;
						break;
						// left edge
						// . . .
						// .|# .
						// . . .
					case 1:
						if (visitedEdges(edgeTileX, edgeTileY) & 0b0100) goto out;
						// bottom left
						// . . .
						// _|# .
						// # . .
						if (adj & 0b00000100) {
							verts.push_back(localToColliderP(edgeTileX - 1, edgeTileY + 1));
							visitedEdges(edgeTileX, edgeTileY) |= 0b0100;
							//m_tiles(edgeTileX, edgeTileY, 3).m_tileID = 3;
							edgeTileX--;
							edgeTileY++;
							currentSide = 0;
							break;
						}
						// bottom
						// . . .
						// .|# .
						// .|# .
						if (adj & 0b00000010) {
							verts.push_back(localToColliderP(edgeTileX, edgeTileY + 2));
							visitedEdges(edgeTileX, edgeTileY) |= 0b0100;
							//m_tiles(edgeTileX, edgeTileY, 3).m_tileID = 3;
							edgeTileY++;
							currentSide = 1;
							break;
						}
						// no tile in direction of traversal
						// . . .
						// .|# .
						// . ‾ .
						verts.push_back(localToColliderP(edgeTileX + 1, edgeTileY + 1));
						visitedEdges(edgeTileX, edgeTileY) |= 0b0100;
						//m_tiles(edgeTileX, edgeTileY, 3).m_tileID = 3;
						currentSide = 2;
						break;
						// bottom edge
						// . . .
						// . # .
						// . ‾ .
					case 2:
						if (visitedEdges(edgeTileX, edgeTileY) & 0b0010) goto out;
						// bottom right
						// . . .
						// . # .
						// . ‾|#
						if (adj & 0b00000001) {
							verts.push_back(localToColliderP(edgeTileX + 1, edgeTileY + 2));
							visitedEdges(edgeTileX, edgeTileY) |= 0b0010;
							//m_tiles(edgeTileX, edgeTileY, 3).m_tileID = 3;
							edgeTileX++;
							edgeTileY++;
							currentSide = 1;
							break;
						}
						// right
						// . . .
						// . # #
						// . ‾ ‾
						if (adj & 0b00001000) {
							verts.push_back(localToColliderP(edgeTileX + 2, edgeTileY + 1));
							visitedEdges(edgeTileX, edgeTileY) |= 0b0010;
							//m_tiles(edgeTileX, edgeTileY, 3).m_tileID = 3;
							edgeTileX++;
							currentSide = 2;
							break;
						}
						// no tile in direction of traversal
						// . . .
						// . #| .
						// . ‾ .
						verts.push_back(localToColliderP(edgeTileX + 1, edgeTileY));
						visitedEdges(edgeTileX, edgeTileY) |= 0b0010;
						currentSide = 3;
						break;
						// right edge
						// . . .
						// . #|.
						// . . .
					case 3:
						if (visitedEdges(edgeTileX, edgeTileY) & 0b0001) goto out;
						// top right
						// . . #
						// . #|‾
						// . . .
						if (adj & 0b00100000) {
							verts.push_back(localToColliderP(edgeTileX + 2, edgeTileY));
							visitedEdges(edgeTileX, edgeTileY) |= 0b0001;
							//m_tiles(edgeTileX, edgeTileY, 3).m_tileID = 3;
							edgeTileX++;
							edgeTileY--;
							currentSide = 2;
							break;
						}
						// top
						// . #|.
						// . #|.
						// . . .
						if (adj & 0b01000000) {
							verts.push_back(localToColliderP(edgeTileX + 1, edgeTileY - 1));
							visitedEdges(edgeTileX, edgeTileY) |= 0b0001;
							//m_tiles(edgeTileX, edgeTileY, 3).m_tileID = 3;
							edgeTileY--;
							currentSide = 3;
							break;
						}
						// no tile in direction of traversal
						// . _ .
						// . #|.
						// . . .
						verts.push_back(localToColliderP(edgeTileX, edgeTileY));
						visitedEdges(edgeTileX, edgeTileY) |= 0b0001;
						//m_tiles(edgeTileX, edgeTileY, 3).m_tileID = 3;
						currentSide = 0;
						break;
					}
				}
			}
		out:;
			if (verts.size() > 3) {
				b2ChainShape shape;
				shape.CreateLoop(verts.data(), (int)verts.size());
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &shape;
				fixtureDef.density = 1.0f;
				fixtureDef.friction = 0.5f;
				fixtureDef.restitution = 0.0f;
				m_collisionBody->CreateFixture(&fixtureDef);
				verts.clear();
			}
		}
	}


	//b2PolygonShape shape;
	//shape.SetAsBox(chunkSizeMeters / 2.f, chunkSizeMeters / 2.f);


	colliderValid = true;
}

void WorldChunk::pushVBO()
{
	std::unique_lock<std::mutex> lock(m_vboMutex);
	tileMesh.pushVBOToGPU();
	//tileMesh.clean();
	vboIsPushed = true;
}

void WorldChunk::subSingleTileVBOS()
{
	while (auto opt = tilesToSub.tryPop()) {
		glm::ivec3 tile = opt.value();
		size_t startIdx = (tile.z * CHUNKSIZE * CHUNKSIZE) + ((tile.y * CHUNKSIZE) + tile.x);
		m_vboMutex.lock();
		tileMesh.subVBOData((GLuint)startIdx, (GLuint)startIdx + 1, &tileMesh.getVerts()[startIdx]);
		m_vboMutex.unlock();
	}
}

uint32_t WorldChunk::getVBOSize() {
	return (uint16_t)tileMesh.getTotalVBOSize();
}

// ill advised now
void WorldChunk::flip()
{
	//m_tiles.invertDepth = !m_tiles.invertDepth;
}


void WorldChunk::setChunkTile(glm::ivec3 p_chunkCoordinates, uint32_t p_tileID) {
	if (m_tiles(p_chunkCoordinates.x, p_chunkCoordinates.y, p_chunkCoordinates.z).m_tileID == p_tileID) return;
	m_tiles(p_chunkCoordinates.x, p_chunkCoordinates.y, p_chunkCoordinates.z) = Tile(p_chunkCoordinates, p_tileID);
	colliderValid = false;
	feedbackMeshReady = false;
	return;
}

Tile& WorldChunk::getChunkTile(size_t p_x, size_t p_y, size_t p_z)
{
	return m_tiles(p_x, p_y, p_z);
}

std::optional<Tile*> WorldChunk::getNeigboringChunkTile(int p_chunkX, int p_chunkY, int p_chunkZ, ChunkManager& p_chnks)
{
	if (!m_tiles.bounded(p_chunkX, p_chunkY, p_chunkZ)) {
		int intrudedChunkX;
		int intrudedChunkY;
		intrudedChunkX = (p_chunkX < 0) ? worldPos.x - 1 :
			(p_chunkX > CHUNKSIZE - 1) ? worldPos.x + 1 :
			worldPos.x;
		intrudedChunkY = (p_chunkY < 0) ? worldPos.y + 1 :
			(p_chunkY > CHUNKSIZE - 1) ? worldPos.y - 1 :
			worldPos.y;
		std::optional<WorldChunk*> opt = p_chnks.getChunkPtr(ChunkPos(intrudedChunkX, intrudedChunkY));
		if (!opt.has_value()) return std::nullopt;
		WorldChunk* neighbor = opt.value();

		Tile& t = neighbor->getChunkTile(
			utils::modUnsigned(p_chunkX, CHUNKSIZE), 
			utils::modUnsigned(p_chunkY, CHUNKSIZE),
			p_chunkZ);

		return &t;
	}
	else {
		return &m_tiles(p_chunkX, p_chunkY, p_chunkZ);
	}
}

std::optional<WorldChunk*> WorldChunk::getIntrudedChunk(int p_localTileX, int p_localTileY, int localTileZ, ChunkManager& p_chnks)
{
		if (!m_tiles.bounded(p_localTileX, p_localTileY, localTileZ)) {
			int intrudedChunkX;
			int intrudedChunkY;
			intrudedChunkX = (p_localTileX < 0) ? worldPos.x - 1 :
				(p_localTileX > CHUNKSIZE - 1) ? worldPos.x + 1 :
				worldPos.x;
			intrudedChunkY = (p_localTileY < 0) ? worldPos.y + 1 :
				(p_localTileY > CHUNKSIZE - 1) ? worldPos.y - 1 :
				worldPos.y;
			std::optional<WorldChunk*> opt = p_chnks.getChunkPtr(ChunkPos(intrudedChunkX, intrudedChunkY));
			if (!opt.has_value()) return std::nullopt;
			return opt.value();
		}
		else {
			return std::nullopt;
		}
}

void WorldChunk::remove() {
	associatedWorld->DestroyBody(m_collisionBody);
	tileMesh.remove();
}

void WorldChunk::draw(DrawSurface& p_target, DrawStates& p_drawStates)
{
	DrawStates newStates = DrawStates(p_drawStates);

	if (!feedbackMesh.feedbackInitDone) {
		feedbackMesh.initFeedbackBuffer(45 * CHUNKSIZE * CHUNKSIZE * CHUNKDEPTH, tileMesh.VAO->ID);
	}
	newStates.setTransform(p_drawStates.m_transform * m_transform);
	if (feedbackMeshReady) {
		//newStates.setTransform(p_drawStates.m_transform);
		p_target.draw(feedbackMesh, GL_TRIANGLES, newStates);
		return;
	}
	feedbackMesh.startFeedback(GL_TRIANGLES);
	p_target.draw(tileMesh, GL_POINTS, newStates, false);
	feedbackMesh.endFeedback();
	feedbackMeshReady = true;
}
