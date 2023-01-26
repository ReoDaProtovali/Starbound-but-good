#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include <util/ext/glm/glm.hpp>
#include <stdexcept>


noise::module::Perlin WorldChunk::s_noiseGenerator;

WorldChunk::WorldChunk(ChunkPos p_chunkPos, int p_worldID) :
	worldPos(p_chunkPos),
	worldID(p_worldID),
	invalid(false),
	m_tiles(CHUNKSIZE, CHUNKSIZE)
{

	setPosition(glm::vec3((float)p_chunkPos.x * CHUNKSIZE, (float)p_chunkPos.y * CHUNKSIZE, 0.f));
	calculateTransform();
	tileMesh.addUintAttrib(1); // xyzID, one uint
	tileMesh.addUbyteAttrib(1);
	s_noiseGenerator.SetOctaveCount(10);
	s_noiseGenerator.SetPersistence(0.54);
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
void WorldChunk::worldGenerate() {
	isEmpty = true;

	ResourceLoader& res = ResourceLoader::Get();
	TileInfo& DirtInfo = res.getTileInfo("vanilla:dirt").value();
	TileInfo& StoneInfo = res.getTileInfo("vanilla:stone").value();


	float surfaceLevel = 10.0f * CHUNKSIZE;
	float globalX;
	float globalY; // tile positions
	float height;
	float height2;
	float caveLayer1;
	float caveLayer2;

	for (int y = 0; y < CHUNKSIZE; y++) {
		for (int x = 0; x < CHUNKSIZE; x++) {
			globalX = (float)(worldPos.x * CHUNKSIZE + x);
			globalY = (float)(worldPos.y * CHUNKSIZE - y) - surfaceLevel;
			// detail level for the surface noise
			s_noiseGenerator.SetOctaveCount(7);
			// surface level
			height = (float)s_noiseGenerator.GetValue(globalX / 200.0f, 214.2f, 2.0f) * 40.0f;
			// The dirt level
			// first noise is a scaled down version of the surface line            second noise is to make a dither effect between the layers
			height2 = (float)s_noiseGenerator.GetValue(globalX / 200.0f, 214.2f, 2.0f) * 20.0f - (float)(5.0f * s_noiseGenerator.GetValue(globalX / 1.2f, globalY / 1.2f, 2.0f));
			float scale1 = 1 / 10.0f;
			float scale2 = 1 / 1.0f;
			s_noiseGenerator.SetOctaveCount(2);
			caveLayer1 = (float)(s_noiseGenerator.GetValue((double)globalX * scale1, (double)globalY * scale1, 2.0) / 2.0);
			s_noiseGenerator.SetOctaveCount(1);
			caveLayer2 = (float)s_noiseGenerator.GetValue(((double)globalX * scale1 * 0.7) + caveLayer1 * scale2, ((double)globalY * scale1 * 1.0) + caveLayer1 * scale2, 40.0f);
			//std::cout << utils::clamp(globalY + surfaceLevel, 0.0f, 10.0f) / 10.0f << std::endl;
			if (caveLayer2 < 0.1f + (utils::clamp(globalY - height + 64.0f, 0.0f, 10.0f) / 10.0f) + utils::clamp(globalY + 300.0f, 0.0f, 300.0f) / 350.0f) {
				if (globalY > height) {
					m_tiles(x, y) = Tile(glm::ivec2(x, y), 0);
				}
				else {
					if (globalY < -40.0f + height2) {
						m_tiles(x, y) = Tile(glm::ivec2(x, y), DirtInfo.imageIndex);
						isEmpty = false;
					}
					else {
						m_tiles(x, y) = Tile(glm::ivec2(x, y), StoneInfo.imageIndex);
						isEmpty = false;
					}
				}
			}
			else {
				m_tiles(x, y) = Tile(glm::ivec2(x, y), 0);
			}
		}
	}
	meshIsCurrent = false;
}

void WorldChunk::generateVBO(ChunkManager& p_chnks) {
	if (isEmpty) return;
	tileMesh.remove();
	tileMesh.reserve((size_t)CHUNKSIZE * CHUNKSIZE * sizeof(TileVert)); // reserve a chunks worth of data idk
	for (int y = 0; y < CHUNKSIZE; y++) {
		for (int x = 0; x < CHUNKSIZE; x++) {
			if (m_tiles(x, y).m_tileID == 0) continue;
			uint32_t tID = m_tiles(x, y).m_tileID;

			TileVert v = TileVert(
				x, y, 0, // Position attributes
				tID // numerical ID
			);

			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {

					// in the case the tile lies on the edge you have to do this garbage
					if (!m_tiles.bounded(x + j, y + i)) {
						int intrudedChunkX;
						int intrudedChunkY;
						intrudedChunkX = (x + j < 0) ? worldPos.x - 1 :
							(x + j > 31) ? worldPos.x + 1 : 
							worldPos.x;
						intrudedChunkY = (y + i < 0) ? worldPos.y + 1 :
							(y + i > 31) ? worldPos.y - 1 : 
							worldPos.y;

						if (p_chnks.chunkExistsAt(ChunkPos(intrudedChunkX, intrudedChunkY))) {
							bool success;
							auto opt = p_chnks.getChunkPtr(ChunkPos(intrudedChunkX, intrudedChunkY), success);
							WorldChunk* neighbor = opt.value();

							Tile& t = neighbor->getChunkTile((int)utils::fwrapUnsigned(x + j, CHUNKSIZE), (int)utils::fwrapUnsigned(y + i, CHUNKSIZE));

							if (t.m_tileID != 0) continue;
						}
						// if ya made it to this point, the tile is empty! :)

						goto out;
					}

					if (j == 0 && i == 0) continue;
					if (m_tiles(x + j, y + i).m_tileID != 0) continue;
					out:
					if (i == -1 && j == -1) { v.setAdjacent(Adjacency::TL); } else
					if (i == -1 && j ==  0) { v.setAdjacent(Adjacency::T); } else
					if (i == -1 && j ==  1) { v.setAdjacent(Adjacency::TR); } else
					if (i ==  0 && j == -1) { v.setAdjacent(Adjacency::L); } else
					if (i ==  0 && j ==  1) { v.setAdjacent(Adjacency::R); } else
					if (i ==  1 && j == -1) { v.setAdjacent(Adjacency::BL); } else
					if (i ==  1 && j ==  0) { v.setAdjacent(Adjacency::B); } else
					if (i ==  1 && j ==  1) { v.setAdjacent(Adjacency::BR); }
				}
			}
			
			tileMesh.pushVertices({ v });
		}
	}

	tileMesh.genVBO();
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
