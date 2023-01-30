#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include <util/ext/glm/glm.hpp>
#include <stdexcept>

WorldChunk::WorldChunk(ChunkPos p_chunkPos, int p_worldID) :
	worldPos(p_chunkPos),
	worldID(p_worldID),
	invalid(false),
	m_tiles(CHUNKSIZE, CHUNKSIZE)
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
	meshIsCurrent(other.meshIsCurrent),
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
	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE, 0.f));
	calculateTransform();
	invalid = other.invalid;
	meshIsCurrent = other.meshIsCurrent;
	isEmpty = other.isEmpty;
	return *this;
};

WorldChunk::WorldChunk(WorldChunk&& other) noexcept :
	tileMesh(std::move(other.tileMesh)),
	m_tiles(std::move(other.m_tiles))
{
	worldID = other.worldID;
	worldPos = other.worldPos;
	setPosition(glm::vec3((float)worldPos.x * CHUNKSIZE, (float)worldPos.y * CHUNKSIZE, 0.f));
	calculateTransform();
	invalid = other.invalid;
	meshIsCurrent = other.meshIsCurrent;
	isEmpty = other.isEmpty;
}

void WorldChunk::fillRandom() {
	isEmpty = false;

	for (int y = 0; y < CHUNKSIZE; y++) {
		for (int x = 0; x < CHUNKSIZE; x++) {
			if (rand() % 10 > 5) {
				m_tiles(x, y) = Tile(glm::ivec2(x, y), 1);
			}
			else {
				m_tiles(x, y) = Tile(glm::ivec2(x, y), 2);
			}
		}
	}
	meshIsCurrent = false;
}
void WorldChunk::worldGenerate(WorldGenNoisemap& noiseGen) {
	isEmpty = true;

	ResourceManager& res = ResourceManager::Get();
	TileInfo& DirtInfo = res.getTileInfo("vanilla:dirt").value();
	TileInfo& StoneInfo = res.getTileInfo("vanilla:stone").value();
	TileInfo& NeonInfo = res.getTileInfo("vanilla:neongrid").value();


	float surfaceLevel = -3.0f * CHUNKSIZE;
	float globalX;
	float globalY;

	for (int y = 0; y < CHUNKSIZE; y++) {
		for (int x = 0; x < CHUNKSIZE; x++) {
			globalX = (float)(worldPos.x * CHUNKSIZE + x);
			globalY = (float)(worldPos.y * CHUNKSIZE - y + surfaceLevel);
			glm::vec4 cavern = noiseGen.getPixel((int)globalX, (int)globalY, "cavern");
			glm::vec4 perlin = noiseGen.getPixel((int)globalX, (int)globalY, "perlin");

			if (cavern.x < 0.5) {
				if (perlin.y > 0.5) {
					m_tiles(x, y) = Tile(glm::ivec2(x, y), StoneInfo.spriteIndex);
				}
				else {
					m_tiles(x, y) = Tile(glm::ivec2(x, y), DirtInfo.spriteIndex);
				}
				isEmpty = false;
			}
			else {
				m_tiles(x, y) = Tile(glm::ivec2(x, y), 0);
			}
			//if (std::sin((float)globalX / 64.f) < globalY / 128.f) {
			//	m_tiles(x, y) = Tile(x, y, 1);
			//}
			//else {
			//	m_tiles(x, y) = Tile(x, y, 0);
			//}
			//isEmpty = false;
		}
	}
	meshIsCurrent = false;
}

void WorldChunk::generateVBO(ChunkManager& p_chnks) {
	if (isEmpty) return;
	tileMesh.clearVerts();
	tileMesh.reserve((size_t)CHUNKSIZE * CHUNKSIZE * sizeof(TileVert)); // reserve a chunks worth of data idk
	ResourceManager& res = ResourceManager::Get();

	for (int y = 0; y < CHUNKSIZE; y++) {
		for (int x = 0; x < CHUNKSIZE; x++) {
			if (m_tiles(x, y).m_tileID == 0) continue;
			uint32_t tID = m_tiles(x, y).m_tileID;

			TileVert v = TileVert(
				x, y, 0, // Position attributes
				tID // numerical ID
			);

			// it's teeechnically possible to convert the image id into the cache id by subtracting one
			TileInfo& tInfo = res.getTileInfo(tID - 1);
			v.variationCount = tInfo.variationCount;

			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {

					// in the case the tile lies on the edge you have to do this garbage
					if (!m_tiles.bounded(x + j, y + i)) {
						int intrudedChunkX;
						int intrudedChunkY;
						intrudedChunkX = (x + j < 0) ? worldPos.x - 1 :
							(x + j > CHUNKSIZE - 1) ? worldPos.x + 1 :
							worldPos.x;
						intrudedChunkY = (y + i < 0) ? worldPos.y + 1 :
							(y + i > CHUNKSIZE - 1) ? worldPos.y - 1 :
							worldPos.y;

						if (p_chnks.chunkExistsAt(ChunkPos(intrudedChunkX, intrudedChunkY))) {
							bool success;
							auto opt = p_chnks.getChunkPtr(ChunkPos(intrudedChunkX, intrudedChunkY), success);
							if (!opt.has_value()) {
								ERROR_LOG("what the fuck");
								return;
							}
							WorldChunk* neighbor = opt.value();

							Tile& t = neighbor->getChunkTile(
								(int)utils::fwrapUnsigned(float(x + j), float(CHUNKSIZE)), // a bit weird to convert back and forth here but whatevs
								(int)utils::fwrapUnsigned(float(y + i), float(CHUNKSIZE)));

							if (t.m_tileID != 0) continue;
						}
						// if ya made it to this point, the tile is empty! :)
						goto out;
					}
					if (j == 0 && i == 0) continue;
					if (m_tiles(x + j, y + i).m_tileID != 0) continue;

				out:

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

			tileMesh.pushVertices({ v });
		}
	}

	tileMesh.genVBO();
	tileMesh.clearVerts();
	meshIsCurrent = true;
}

uint32_t WorldChunk::getVBOSize() {
	return (uint16_t)tileMesh.getTotalVBOSize();
}

Tile* WorldChunk::getTiles() {
	return m_tiles.getData().data();
}

void WorldChunk::setChunkTile(glm::ivec2 p_chunkCoordinates, uint32_t p_tileID) {
	m_tiles(p_chunkCoordinates.x, p_chunkCoordinates.y) = Tile(p_chunkCoordinates, p_tileID);
	return;
}

Tile& WorldChunk::getChunkTile(int p_x, int p_y)
{
	return m_tiles(p_x, p_y);
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
