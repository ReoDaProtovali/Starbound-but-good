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
	setPosition(glm::vec3((float)p_chunkPos.x * CHUNKSIZE, (float)p_chunkPos.y * CHUNKSIZE - CHUNKSIZE, 0.f));
	calculateTransform();
	//tileMesh.addUintAttrib(1); // xyzID, one uint
	//tileMesh.addUintAttrib(1); // adjacencies
	//tileMesh.addUintAttrib(1); // variation count i guess
	tileMesh.addFloatAttrib(3); // pos
	tileMesh.addFloatAttrib(2); // texcoord
	tileMesh.addFloatAttrib(4); // lightingCol

	borderMesh.addFloatAttrib(3); // pos
	borderMesh.addFloatAttrib(2); // texCoord
	borderMesh.addFloatAttrib(4); // lightingCol

	//tileMesh.addUintAttrib(1); // variation count
	//tileMesh.addUintAttrib(1); // flags
	//feedbackMesh.addFloatAttrib(2);
	//feedbackMesh.addFloatAttrib(1);
	//feedbackMesh.addFloatAttrib(4);
	//feedbackMesh.setStreamType(GL_DYNAMIC_DRAW);
}

WorldChunk::WorldChunk(const WorldChunk& other) noexcept :
	tileMesh(other.tileMesh),
	borderMesh(other.borderMesh),
	m_tiles(other.m_tiles),
	worldID(other.worldID),
	worldPos(other.worldPos),
	invalid(other.invalid.load()),
	vboIsPushed(other.vboIsPushed.load()),
	isEmpty(other.isEmpty.load())
{
	// Don't forget the base class
	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE - CHUNKSIZE, 0.f));
	calculateTransform();
}

WorldChunk& WorldChunk::operator=(const WorldChunk& other)
{

	tileMesh = other.tileMesh;
	borderMesh = other.borderMesh;
	m_tiles = other.m_tiles;
	worldID = other.worldID;
	worldPos = other.worldPos;
	vboIsPushed = other.vboIsPushed.load();
	invalid = other.invalid.load();
	isEmpty = other.isEmpty.load();

	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE - CHUNKSIZE, 0.f));
	calculateTransform();

	return *this;
}
WorldChunk& WorldChunk::operator=(WorldChunk&& other) noexcept
{
	tileMesh = std::move(other.tileMesh);
	borderMesh = std::move(other.borderMesh);
	m_tiles = std::move(other.m_tiles);
	worldID = other.worldID;
	worldPos = other.worldPos;
	vboIsPushed = other.vboIsPushed.load();
	invalid = other.invalid.load();
	isEmpty = other.isEmpty.load();

	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE - CHUNKSIZE, 0.f));
	calculateTransform();

	return *this;
}
;

WorldChunk::WorldChunk(WorldChunk&& other) noexcept :
	tileMesh(std::move(other.tileMesh)),
	borderMesh(std::move(other.borderMesh)),
	m_tiles(std::move(other.m_tiles)),
	worldID(other.worldID),
	worldPos(other.worldPos),
	vboIsPushed(other.vboIsPushed.load()),
	invalid(other.invalid.load()),
	isEmpty(other.isEmpty.load())
{
	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE - CHUNKSIZE, 0.f));
	calculateTransform();
}

void WorldChunk::fillRandom() {

	for (int z = 0; z < CHUNKDEPTH; z++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int x = 0; x < CHUNKSIZE; x++) {
				if (rand() % 10 > 5) {
					m_tiles(x, y, z) = Tile(1);
				}
				else {
					m_tiles(x, y, z) = Tile(2);
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
	vboBusy = true; // a bad way of preventing half-baked pushes
	//if (isEmpty) return;
	tileMesh.remove();
	borderMesh.remove();
	//tileMesh.reserve((size_t)CHUNKSIZE * CHUNKSIZE * CHUNKDEPTH * sizeof(TileVert)); // reserve a chunks worth of data idk
	ResourceManager& res = ResourceManager::Get();

	Texture tileSheet = res.getTileSheetTexture();

	auto toTexCoords = [&](uint32_t x, uint32_t y) -> glm::vec2 {
		return { 1.f - float(x) / tileSheet.width, 1.f - float(y) / tileSheet.height };

		};
	auto pushBorderVerts = [&](Rect pos_rect, Rect tex_rect, glm::vec4 lightingCol, GLfloat z) {
		borderMesh.pushVertices({
				{{ pos_rect.getTL().x, pos_rect.getTL().y,  z }, {tex_rect.getBR().x, tex_rect.getBR().y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w}},
				{{ pos_rect.getBL().x, pos_rect.getBL().y , z }, {tex_rect.getTR().x, tex_rect.getTR().y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w} },
				{{ pos_rect.getTR().x, pos_rect.getTR().y, z }, {tex_rect.getBL().x, tex_rect.getBL().y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w} },
				{{ pos_rect.getBL().x, pos_rect.getBL().y, z }, {tex_rect.getTR().x, tex_rect.getTR().y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w} },
				{{ pos_rect.getTR().x, pos_rect.getTR().y, z }, {tex_rect.getBL().x, tex_rect.getBL().y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w} },
				{{ pos_rect.getBR().x, pos_rect.getBR().y, z }, {tex_rect.getTL().x, tex_rect.getTL().y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w} }
			});

		};

	for (int z = 0; z < CHUNKDEPTH; z++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int x = 0; x < CHUNKSIZE; x++) {
				uint32_t tID = m_tiles(x, y, z).m_tileID;
				if (tID == 0) continue;


				// Remove non-visible tiles and fix borders

				std::vector<bool> cullMask = { true, true, true, true, true, true, true, true };

				bool flag = false;
				for (int layer = CHUNKDEPTH - 1; layer > z; layer--) {
					uint8_t tileCount = 0;
					for (int i = -1; i <= 1; i++) {
						for (int j = -1; j <= 1; j++) {
							auto opt = getNeigboringChunkTile(x + j, y + i, layer, p_chnks);
							if (opt.has_value()) {
								if (!opt.value()) continue;
								if (opt.value()->m_tileID == 0) continue;
								if (res.getTileInfo(opt.value()->m_tileID - 1).transparent) continue;
								int flatIndex = (1 + j + (1 + i) * 3);
								if (flatIndex != 4)
									cullMask[(flatIndex > 4 ? flatIndex - 1 : flatIndex)] = false;
								tileCount++;
							}
						}
					}

					if (tileCount == 9) {
						flag = true;
					}
				}

				if (flag) continue;
				// it's teeechnically possible to convert the image id into the cache id by subtracting one
				TileInfo& tInfo = res.getTileInfo(tID - 1);
				uint32_t variationCount = tInfo.variationCount;
				glm::vec4 lightingCol = { tInfo.lightingColor[0], tInfo.lightingColor[1], tInfo.lightingColor[2], 1.f - tInfo.lightAbsorption };

				// evil hash code
				float tmp = sinf(glm::dot(glm::vec2(float(worldPos.x * CHUNKSIZE + x) / CHUNKSIZE, float(worldPos.y * CHUNKSIZE + y) / CHUNKSIZE), glm::vec2{ 12.9898, 78.233 }) * 43758.5453);
				float hash1 = fabs(tmp) - (long)fabs(tmp);
				uint32_t variationHash = uint32_t(0xFA5A3 * hash1);


				// in the future, allow multiple variation rows
				uint32_t variation = variationHash % variationCount;


				// begin the long list of messy declarations, its just data
				constexpr uint32_t BASE_TILE_DIM = 8;
				constexpr uint32_t BASE_TILE_X = 4;
				constexpr uint32_t BASE_TILE_Y = 12;
				constexpr uint32_t INCORNER_DIM = 4;
				constexpr uint32_t BORDER_WIDTH = 4;
				constexpr uint32_t ROW_HEIGHT = 24;
				constexpr uint32_t COLUMN_WIDTH = 16;

				const uint32_t VARIATION_TILE_X = BASE_TILE_X + variation * COLUMN_WIDTH;
				const uint32_t VARIATION_TILE_Y = BASE_TILE_Y + tID * ROW_HEIGHT;
				const uint32_t VARIATION_INCORNER_X = variation * COLUMN_WIDTH + 4;
				const uint32_t VARIATION_INCORNER_Y = tID * ROW_HEIGHT + 24;

				const glm::vec2 VERTICAL_SIDE_WH = { BORDER_WIDTH / float(tileSheet.width), BASE_TILE_DIM / float(tileSheet.height) };
				const glm::vec2 HORIZONT_SIDE_WH = { BASE_TILE_DIM / float(tileSheet.width), BORDER_WIDTH / float(tileSheet.height) };
				const glm::vec2 CORNER_WH = { INCORNER_DIM / float(tileSheet.width), INCORNER_DIM / float(tileSheet.height) };


				const Rect SIDE_LEFT_UV{   toTexCoords(VARIATION_TILE_X,                                VARIATION_TILE_Y                ), VERTICAL_SIDE_WH };
				const Rect SIDE_RIGHT_UV{  toTexCoords(VARIATION_TILE_X + BASE_TILE_DIM + BORDER_WIDTH, VARIATION_TILE_Y                ), VERTICAL_SIDE_WH };
				const Rect SIDE_TOP_UV{    toTexCoords(VARIATION_TILE_X + BASE_TILE_DIM,                VARIATION_TILE_Y + BORDER_WIDTH ), HORIZONT_SIDE_WH };
				const Rect SIDE_BOTTOM_UV{ toTexCoords(VARIATION_TILE_X + BASE_TILE_DIM,                VARIATION_TILE_Y - BASE_TILE_DIM), HORIZONT_SIDE_WH };

				const Rect OUTCORNER_TL_UV{ toTexCoords(VARIATION_TILE_X,                                VARIATION_TILE_Y + INCORNER_DIM),  CORNER_WH };
				const Rect OUTCORNER_TR_UV{ toTexCoords(VARIATION_TILE_X + BASE_TILE_DIM + INCORNER_DIM, VARIATION_TILE_Y + INCORNER_DIM),  CORNER_WH };
				const Rect OUTCORNER_BL_UV{ toTexCoords(VARIATION_TILE_X,                                VARIATION_TILE_Y - BASE_TILE_DIM), CORNER_WH };
				const Rect OUTCORNER_BR_UV{ toTexCoords(VARIATION_TILE_X + BASE_TILE_DIM + INCORNER_DIM, VARIATION_TILE_Y - BASE_TILE_DIM), CORNER_WH };

				const Rect INCORNER_TL_UV{ toTexCoords(VARIATION_INCORNER_X + INCORNER_DIM, VARIATION_INCORNER_Y - INCORNER_DIM),  CORNER_WH };
				const Rect INCORNER_TR_UV{ toTexCoords(VARIATION_INCORNER_X,                VARIATION_INCORNER_Y - INCORNER_DIM),  CORNER_WH };
				const Rect INCORNER_BL_UV{ toTexCoords(VARIATION_INCORNER_X + INCORNER_DIM, VARIATION_INCORNER_Y),                 CORNER_WH };
				const Rect INCORNER_BR_UV{ toTexCoords(VARIATION_INCORNER_X,                VARIATION_INCORNER_Y),                 CORNER_WH };

				// i have to invert y in the mesh code for... who knows what reason
				// bt is the base tile, it's always rendered regardless of adjacencies, it shall serve as the basis for all other positions
				const glm::vec3 pos_bt_tl = glm::vec3(x,     CHUNKSIZE - 1 - y,     z);
				const glm::vec3 pos_bt_tr = glm::vec3(x + 1, CHUNKSIZE - 1 - y,     z);
				const glm::vec3 pos_bt_bl = glm::vec3(x,     CHUNKSIZE - 1 - y + 1, z);
				const glm::vec3 pos_bt_br = glm::vec3(x + 1, CHUNKSIZE - 1 - y + 1, z);

				const Rect SIDE_LEFT_POS{ {pos_bt_tl.x - 0.5f, pos_bt_tl.y}, {0.5f, 1.f} };
				const Rect SIDE_RIGHT_POS{ {pos_bt_tr.x, pos_bt_tl.y}, {0.5f, 1.f} };
				const Rect SIDE_TOP_POS{ {pos_bt_bl.x, pos_bt_bl.y}, {1.f, 0.5f} };
				const Rect SIDE_BOTTOM_POS{ {pos_bt_tl.x, pos_bt_tl.y - 0.5f}, {1.f, 0.5f} };

				const Rect OUTCORNER_TL_POS{ {pos_bt_tl.x - 0.5f, pos_bt_tl.y + 1.0f}, { 0.5f, 0.5f } };
				const Rect OUTCORNER_TR_POS{ {pos_bt_tr.x,        pos_bt_tr.y + 1.0f}, { 0.5f, 0.5f } };
				const Rect OUTCORNER_BL_POS{ {pos_bt_bl.x - 0.5f, pos_bt_bl.y - 1.5f}, { 0.5f, 0.5f } };
				const Rect OUTCORNER_BR_POS{ {pos_bt_br.x,        pos_bt_br.y - 1.5f}, { 0.5f, 0.5f } };

				const Rect INCORNER_BL_POS{ {pos_bt_tl.x - 0.5f, pos_bt_tl.y + 0.5f}, { 0.5f, 0.5f } };
				const Rect INCORNER_BR_POS{ {pos_bt_tr.x,        pos_bt_tl.y + 0.5f}, { 0.5f, 0.5f } };
				const Rect INCORNER_TR_POS{ {pos_bt_bl.x,        pos_bt_bl.y},        { 0.5f, 0.5f } };
				const Rect INCORNER_TL_POS{ {pos_bt_bl.x + 0.5f, pos_bt_bl.y},        { 0.5f, 0.5f } };

				const glm::vec2 tex_bl = toTexCoords(VARIATION_TILE_X,                 VARIATION_TILE_Y);
				const glm::vec2 tex_br = toTexCoords(VARIATION_TILE_X + BASE_TILE_DIM, VARIATION_TILE_Y);
				const glm::vec2 tex_tl = toTexCoords(VARIATION_TILE_X,                 VARIATION_TILE_Y - BASE_TILE_DIM);
				const glm::vec2 tex_tr = toTexCoords(VARIATION_TILE_X + BASE_TILE_DIM, VARIATION_TILE_Y - BASE_TILE_DIM);


				std::vector<bool> adjacencies = { false, false, false, false, false, false, false, false };
				// Set tile adjacencies
				for (int i = 0; i < 9; i++) {
					if (i == 4) continue;

					auto opt = getNeigboringChunkTile(x + (i % 3) - 1, y + (i / 3) - 1, z, p_chnks);


					if (opt.has_value()) {
						if (opt.value()->m_tileID != 0) continue;
						
					}

					adjacencies[(i > 4 ? i - 1 : i)] = true;
					

				}

				tileMesh.pushVertices({ // push all the calculated tile vertices
					{{pos_bt_tl.x, pos_bt_tl.y, (float)z}, // Position attributes
					{tex_tl.x, tex_tl.y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w}},           // ID Attribute, variationCount
					{{pos_bt_bl.x, pos_bt_bl.y, (float)z},
					{tex_bl.x, tex_bl.y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w}},
					{{pos_bt_tr.x, pos_bt_tr.y, (float)z},
					{tex_tr.x, tex_tr.y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w}},
					{{pos_bt_bl.x, pos_bt_bl.y, (float)z},
					{tex_bl.x, tex_bl.y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w}},
					{{pos_bt_tr.x, pos_bt_tr.y, (float)z},
					{tex_tr.x, tex_tr.y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w}},
					{{pos_bt_br.x, pos_bt_br.y, (float)z},
					{tex_br.x, tex_br.y}, {lightingCol.x, lightingCol.y, lightingCol.z, lightingCol.w}}
					});

				// 0 1 2
				// 3   4
				// 5 6 7


				//// sides
				if (adjacencies[1] && cullMask[1])
					pushBorderVerts(SIDE_TOP_POS, SIDE_TOP_UV, glm::vec4(0.f), z);
				if (adjacencies[6] && cullMask[6])
					pushBorderVerts(SIDE_BOTTOM_POS, SIDE_BOTTOM_UV, glm::vec4(0.f), z);
				if (adjacencies[3] && cullMask[3])
					pushBorderVerts(SIDE_LEFT_POS, SIDE_LEFT_UV, glm::vec4(0.f), z);
				if (adjacencies[4] && cullMask[4])
					pushBorderVerts(SIDE_RIGHT_POS, SIDE_RIGHT_UV, glm::vec4(0.f), z);

				//// --------------------------------------------------------------------

				// pos: left of BT
				if (adjacencies[3] && !adjacencies[0] && cullMask[3])
					pushBorderVerts(INCORNER_BL_POS, INCORNER_BL_UV, glm::vec4(0.f), z);

				// pos: top of BT
				if (adjacencies[1] && !adjacencies[0] && cullMask[1])
					pushBorderVerts(INCORNER_TR_POS, INCORNER_TR_UV, glm::vec4(0.f), z);

				// pos: top of BT
				if (adjacencies[1] && !adjacencies[2] && cullMask[1])
					pushBorderVerts(INCORNER_TL_POS, INCORNER_TL_UV, glm::vec4(0.f), z);

				// pos: right of BT
				if (adjacencies[4] && !adjacencies[2] && cullMask[4])
					pushBorderVerts(INCORNER_BR_POS, INCORNER_BR_UV, glm::vec4(0.f), z);

				//// --------------------------------------------------------------------

				if (adjacencies[1] && adjacencies[4] && adjacencies[2] && cullMask[2])
					pushBorderVerts(OUTCORNER_TR_POS, OUTCORNER_TR_UV, glm::vec4(0.f), z);

				if (adjacencies[1] && adjacencies[3] && adjacencies[0] && cullMask[0])
					pushBorderVerts(OUTCORNER_TL_POS, OUTCORNER_TL_UV, glm::vec4(0.f), z);

				if (adjacencies[6] && adjacencies[3] && adjacencies[5] && cullMask[5])
					pushBorderVerts(OUTCORNER_BL_POS, OUTCORNER_BL_UV, glm::vec4(0.f), z);


				if (adjacencies[6] && adjacencies[4] && adjacencies[7] && cullMask[7])
					pushBorderVerts(OUTCORNER_BR_POS, OUTCORNER_BR_UV, glm::vec4(0.f), z);
			}
		}
	}

	vboIsPushed = false;
	vboBusy = false;

}

//void WorldChunk::genSingleTileVBO(int p_tileX, int p_tileY, int p_tileZ, ChunkManager& p_chnks)
//{
//	ResourceManager& res = ResourceManager::Get();
//	// this is mostly dumb stuff i have to do for the edge case
//	auto tileOpt = getNeigboringChunkTile(p_tileX, p_tileY, p_tileZ, p_chnks);
//	if (!tileOpt.has_value()) return;
//	Tile* thisTile = tileOpt.value();
//	auto npos = utils::worldToLocalChunkCoords(p_tileX, p_tileY);
//	int x = npos.x;
//	int y = npos.y;
//
//	int z = p_tileZ;
//
//	WorldChunk* currentChunk = this;
//	// this will be the case if the tile is outside of this chunk
//	if (x != p_tileX || y != p_tileY) {
//		if (getIntrudedChunk(p_tileX, p_tileY, z, p_chnks).has_value()) {
//			currentChunk = getIntrudedChunk(p_tileX, p_tileY, z, p_chnks).value();
//		}
//	}
//	std::unique_lock<std::mutex> lock(currentChunk->m_vboMutex);
//	//if (currentChunk->isEmpty) return;
//	if (currentChunk->invalid) return;
//	if (currentChunk->tileMesh.getTotalVBOSize() == 0) return;
//	uint32_t tID = thisTile->m_tileID;
//
//	TileVert v = TileVert(
//		x, y, z, // Position attributes
//		tID // numerical ID
//	);
//
//
//	// it's teeechnically possible to convert the image id into the cache id by subtracting one
//	if (tID != 0) {
//		TileInfo& tInfo = res.getTileInfo(tID - 1);
//		v.variationCount = tInfo.variationCount;
//	}
//	// Set tile adjacencies
//	for (int i = 0; i < 9; i++) {
//		if (i == 4) continue;
//
//		auto opt = currentChunk->getNeigboringChunkTile(x + (i % 3) - 1, y + (i / 3) - 1, z, p_chnks);
//
//		auto centerOpt = currentChunk->getNeigboringChunkTile(x, y, z, p_chnks);
//
//		if (opt.has_value()) {
//			if (opt.value()->m_tileID != 0) continue;
//		}
//
//		v.setAdjacent(128u >> (i > 4 ? i - 1 : i));
//
//	}
//	// only doing this because too lazy to do 3d array access math lol
//	StaticArray3D<TileVert> tmpArr(CHUNKSIZE, CHUNKSIZE, CHUNKDEPTH);
//	tmpArr.setData(currentChunk->tileMesh.getVerts().data());
//	tmpArr(x, y, z) = v;
//	currentChunk->tilesToSub.push(glm::ivec3(x, y, z));
//	// so it doesn't self delete
//	tmpArr.setData(nullptr);
//}

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
	if (m_collisionBody) {
		m_collisionBody->DestroyFixture(m_collisionBody->GetFixtureList());
		p_world.DestroyBody(m_collisionBody);

	}
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
	borderMesh.pushVBOToGPU();
	//tileMesh.clean();
	vboIsPushed = true;
}

//void WorldChunk::subSingleTileVBOS()
//{
//	while (auto opt = tilesToSub.tryPop()) {
//		glm::ivec3 tile = opt.value();
//		size_t startIdx = (tile.z * CHUNKSIZE * CHUNKSIZE) + ((tile.y * CHUNKSIZE) + tile.x);
//		m_vboMutex.lock();
//		tileMesh.subVBOData((GLuint)startIdx, (GLuint)startIdx + 1, &tileMesh.getVerts()[startIdx]);
//		feedbackMeshReady = false;
//		m_vboMutex.unlock();
//	}
//}

uint32_t WorldChunk::getTileVBOSize() {
	return (uint16_t)tileMesh.getTotalVBOSize();
}

void WorldChunk::setChunkTile(glm::ivec3 p_chunkCoordinates, uint32_t p_tileID) {
	if (m_tiles(p_chunkCoordinates.x, p_chunkCoordinates.y, p_chunkCoordinates.z).m_tileID == p_tileID) return;
	m_tiles(p_chunkCoordinates.x, p_chunkCoordinates.y, p_chunkCoordinates.z) = Tile(p_tileID);
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
	borderMesh.remove();
}

void WorldChunk::draw(DrawSurface& p_target, DrawStates& p_drawStates)
{
	DrawStates newStates = DrawStates(p_drawStates);

	newStates.setTransform(p_drawStates.m_transform * m_transform);

	newStates.m_blendMode.disable();
	glEnable(GL_DEPTH_TEST);
	p_target.draw(tileMesh, GL_TRIANGLES, newStates, false);

	newStates.m_blendMode.enable();
	glDisable(GL_DEPTH_TEST);
	p_target.draw(borderMesh, GL_TRIANGLES, newStates, false);


}
