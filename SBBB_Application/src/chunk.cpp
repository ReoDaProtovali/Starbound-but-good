#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include <util/ext/glm/glm.hpp>
#include <stdexcept>
#include "GameConstants.hpp"
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
}

WorldChunk::WorldChunk(const WorldChunk& other) noexcept :
	tileMesh(other.tileMesh),
	m_tiles(other.m_tiles),
	worldID(other.worldID),
	worldPos(other.worldPos),
	invalid(other.invalid),
	vertsAreCurrent(other.vertsAreCurrent),
	vboIsPushed(other.vboIsPushed),
	isEmpty(other.isEmpty)
{
	// Don't forget the base class
	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE, 0.f));
	calculateTransform();
}

WorldChunk& WorldChunk::operator=(const WorldChunk& other)
{

	tileMesh = other.tileMesh;
	m_tiles = other.m_tiles;
	worldID = other.worldID;
	worldPos = other.worldPos;
	vertsAreCurrent = other.vertsAreCurrent;
	vboIsPushed = other.vboIsPushed;
	invalid = other.invalid;
	isEmpty = other.isEmpty;

	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE, 0.f));
	calculateTransform();

	return *this;
};

WorldChunk::WorldChunk(WorldChunk&& other) noexcept :
	tileMesh(std::move(other.tileMesh)),
	m_tiles(std::move(other.m_tiles)),
	worldID(other.worldID),
	worldPos(other.worldPos),
	vertsAreCurrent(other.vertsAreCurrent),
	vboIsPushed(other.vboIsPushed),
	invalid(other.invalid),
	isEmpty(other.isEmpty)
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
	vertsAreCurrent = false;
	isEmpty = false;
}
void WorldChunk::worldGenerate(WorldGenNoisemap& noiseGen) {
	isEmpty = true;
	DebugStats& db = DebugStats::Get();
	db.chunkGenCounter++;
	ResourceManager& res = ResourceManager::Get();
	TileInfo& DirtInfo = res.getTileInfo("vanilla:dirt").value();
	TileInfo& StoneInfo = res.getTileInfo("vanilla:stone").value();
	TileInfo& NeonInfo = res.getTileInfo("vanilla:neongrid").value();


	float surfaceLevel = -1.5f * CHUNKSIZE;
	float globalX;
	float globalY;

	for (int z = 0; z < CHUNKDEPTH; z++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int x = 0; x < CHUNKSIZE; x++) {
				globalX = (float)(worldPos.x * CHUNKSIZE + x);
				globalY = (float)(worldPos.y * CHUNKSIZE - y);
				glm::vec4 perlin = noiseGen.getPixel((int)globalX, (int)globalY, "perlin");

				glm::vec4 perlin1d = noiseGen.getPixel((int)globalX, (int)globalY, "perlin1d");

				float height = (perlin1d.x + perlin1d.y * 0.3f + perlin1d.z * 0.4f + perlin1d.w * 0.2f * perlin1d.w - 0.9f) * 300.f + 100.f;
				glm::vec4 cavern = noiseGen.getPixel((int)globalX, (int)globalY, "cavern");


				if (z == 3) {
					if (cavern.x < 0.2 && globalY < height) {
						if (height - globalY > 30 + rand() % 10) {
							if (perlin.y > 0.7 && height - globalY < 180 + rand() % 5) {
								m_tiles(x, y, z) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
							}
							else {
								if (perlin.w > 0.8) {
									m_tiles(x, y, z) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
								}
								else {
									m_tiles(x, y, z) = Tile(glm::ivec2(x, y), StoneInfo.spriteIndex);
								}
							}
						}
						else {
							m_tiles(x, y, z) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
						}
						isEmpty = false;
					}
				}
				else if (z == 2) {
					if (cavern.x < 0.4 && globalY < height) {
						if (height - globalY > 30 + rand() % 10) {
							if (perlin.z > 0.8) {
								m_tiles(x, y, z) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
							}
							else {
								if (perlin.w > 0.5) {
									m_tiles(x, y, z) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
								}
								else {
									m_tiles(x, y, z) = Tile(glm::ivec2(x, y), StoneInfo.spriteIndex);
								}
							}
						}
						else {
							m_tiles(x, y, z) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
						}
						isEmpty = false;
					}
				} 
				else if (z == 1) {
					if (cavern.x < 0.6 && globalY < height) {
						if (height - globalY > -30 + rand() % 10) {
							if (perlin.z > 0.5 && height - globalY < 90 + rand() % 5) {
								m_tiles(x, y, z) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
							}
							else {
								if (perlin.w > 0.4) {
									m_tiles(x, y, z) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
								}
								else {
									m_tiles(x, y, z) = Tile(glm::ivec2(x, y), StoneInfo.spriteIndex);
								}
							}
						}
						else {
							m_tiles(x, y, z) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
						}
						isEmpty = false;
					}
				}
				else if (z == 0) {
					if (height - globalY > 10) {
						if (height - globalY > 30 + rand() % 10) {
							if (perlin.w > 0.2) {
								m_tiles(x, y, z) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
							}
							else {
								m_tiles(x, y, z) = Tile(glm::ivec2(x, y), StoneInfo.spriteIndex);
							}

						}
						else {
							m_tiles(x, y, z) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
						}
					}
					isEmpty = false;
				}
				else {
					m_tiles(x, y, z) = Tile(glm::ivec2(x, y), 0);
				}
			}
		}
	}
	vertsAreCurrent = false;
	invalid = false;
}

void WorldChunk::generateVBO(ChunkManager& p_chnks) {
	if (isEmpty) return;
	tileMesh.clean();
	tileMesh.reserve((size_t)CHUNKSIZE * CHUNKSIZE * CHUNKDEPTH * sizeof(TileVert)); // reserve a chunks worth of data idk
	ResourceManager& res = ResourceManager::Get();
	auto& db = DebugStats::Get();

	for (int z = 0; z < CHUNKDEPTH; z++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int x = 0; x < CHUNKSIZE; x++) {
				if (m_tiles(x, y, z).m_tileID == 0) continue;
				uint32_t tID = m_tiles(x, y, z).m_tileID;

				TileVert v = TileVert(
					x, y, z, // Position attributes
					tID // numerical ID
				);

				// it's teeechnically possible to convert the image id into the cache id by subtracting one
				TileInfo& tInfo = res.getTileInfo(tID - 1);
				v.variationCount = tInfo.variationCount;

				// Remove non-visible tiles
				for (int layer = CHUNKDEPTH - 1; layer > z; layer--) {
					uint8_t tileCount = 0;
					for (int i = -1; i <= 1; i++) {
						for (int j = -1; j <= 1; j++) {
							auto opt = getNeigboringChunkTile(x + j, y + i, layer, p_chnks);
							if (opt.has_value()) {
								if (!opt.value()) continue;
								if (opt.value()->m_tileID == 0) continue;
								if (res.getTileInfo(opt.value()->m_tileID - 1).transparent) continue;
								if (opt.value()->m_tileID != 0) tileCount++;
							}
						}
					}

					if (tileCount == 9) {
						goto skip;
					}
				}

				// Set tile adjacencies
				for (int i = -1; i <= 1; i++) {
					for (int j = -1; j <= 1; j++) {
						if (j == 0 && i == 0) continue;

						// in the case the tile lies on the edge you have to do this garbage
						auto opt = getNeigboringChunkTile(x + j, y + i, z, p_chnks);
						if (opt.has_value()) {
							if (opt.value()->m_tileID != 0) continue;
						}

						if (i == -1 && j == -1) {
							v.setAdjacent(Adjacency::TL);
						}
						else if (i == -1 && j == 0) {
							v.setAdjacent(Adjacency::T);
						}
						else if (i == -1 && j == 1) {
							v.setAdjacent(Adjacency::TR);
						}
						else if (i == 0 && j == -1) {
							v.setAdjacent(Adjacency::L);
						}
						else if (i == 0 && j == 1) {
							v.setAdjacent(Adjacency::R);
						}
						else if (i == 1 && j == -1) {
							v.setAdjacent(Adjacency::BL);
						}
						else if (i == 1 && j == 0) {
							v.setAdjacent(Adjacency::B);
						}
						else if (i == 1 && j == 1) {
							v.setAdjacent(Adjacency::BR);
						}
					}
				}
				tileMesh.pushVertex(v);
				db.vertCount++;
			skip:
				{}
			}
		}
	}
	vertsAreCurrent = true;
	vboIsPushed = false;
}

void WorldChunk::pushVBO()
{
	tileMesh.pushVBOToGPU();
	tileMesh.clean();
	vboIsPushed = true;
}

uint32_t WorldChunk::getVBOSize() {
	return (uint16_t)tileMesh.getTotalVBOSize();
}

Tile* WorldChunk::getTiles() {
	return m_tiles.getData().data();
}

void WorldChunk::setChunkTile(glm::ivec3 p_chunkCoordinates, uint32_t p_tileID) {
	m_tiles(p_chunkCoordinates.x, p_chunkCoordinates.y, p_chunkCoordinates.z) = Tile(p_chunkCoordinates, p_tileID);
	return;
}

Tile& WorldChunk::getChunkTile(int p_x, int p_y, int p_z)
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
			(int)utils::fwrapUnsigned(float(p_chunkX), float(CHUNKSIZE)), // a bit weird to convert back and forth here but whatevs
			(int)utils::fwrapUnsigned(float(p_chunkY), float(CHUNKSIZE)),
			p_chunkZ);

		return &t;
	}
	else {
		return &m_tiles(p_chunkX, p_chunkY, p_chunkZ);
	}
}

void WorldChunk::remove() {
	tileMesh.remove();
}

void WorldChunk::draw(DrawSurface& p_target, DrawStates& p_drawStates)
{
	DrawStates newStates = DrawStates(p_drawStates);

	newStates.setTransform(p_drawStates.m_transform * m_transform);

	p_target.draw(tileMesh, GL_POINTS, newStates);

}
