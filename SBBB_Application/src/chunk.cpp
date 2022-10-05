#include "Chunk.hpp"
#include <util/ext/glm/glm.hpp>
#include <stdexcept>

noise::module::Perlin WorldChunk::noiseGenerator;

WorldChunk::WorldChunk(ChunkPos p_chunkPos, int p_worldID) : 
	worldPos(p_chunkPos),
	worldID(p_worldID),
	invalid(false),
	tiles(chunkSize, chunkSize, Tile())
{

	setPosition(glm::vec3((float)p_chunkPos.x * chunkSize, (float)p_chunkPos.y * chunkSize, 0.f));
	calculateTransform();
	tileMesh.addUintAttrib(1); // xyzID, one uint

	//fillRandom();
	//generateVBO();
	noiseGenerator.SetOctaveCount(10);
	noiseGenerator.SetPersistence(0.54);
}
void WorldChunk::fillRandom() {
	isEmpty = false;

	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++) {
			if (rand() % 10 > 5) {
				tiles(x, y) = Tile(glm::ivec2(x, y), 1);
			}
			else {
				tiles(x, y) = Tile(glm::ivec2(x, y), 2);
			}
		}
	}
	meshIsCurrent = false;
}
void WorldChunk::worldGenerate() {
	isEmpty = true;

	float surfaceLevel = 10.0f * chunkSize;
	float globalX;
	float globalY; // tile positions
	float height;
	float height2;
	float caveLayer1;
	float caveLayer2;

	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++) {
			globalX = (float)(worldPos.x * chunkSize + x);
			globalY = (float)(worldPos.y * chunkSize - y) - surfaceLevel;
			// detail level for the surface noise
			noiseGenerator.SetOctaveCount(7);
			// surface level
			height = (float)noiseGenerator.GetValue(globalX / 200.0f, 214.2f, 2.0f) * 40.0f;
			// The dirt level
			// first noise is a scaled down version of the surface line            second noise is to make a dither effect between the layers
			height2 = (float)noiseGenerator.GetValue(globalX / 200.0f, 214.2f, 2.0f) * 20.0f - (float)(5.0f * noiseGenerator.GetValue(globalX / 1.2f, globalY / 1.2f, 2.0f));
			float scale1 = 1 / 10.0f;
			float scale2 = 1 / 1.0f;
			noiseGenerator.SetOctaveCount(2);
			caveLayer1 = (float)(noiseGenerator.GetValue((double)globalX * scale1, (double)globalY * scale1, 2.0) / 2.0);
			noiseGenerator.SetOctaveCount(1);
			caveLayer2 = (float)noiseGenerator.GetValue(((double)globalX * scale1 * 0.7) + caveLayer1 * scale2, ((double)globalY * scale1 * 1.0) + caveLayer1 * scale2, 40.0f);
			//std::cout << utils::clamp(globalY + surfaceLevel, 0.0f, 10.0f) / 10.0f << std::endl;
			if (caveLayer2 < 0.05f + (utils::clamp(globalY - height + 64.0f, 0.0f, 10.0f) / 10.0f) + utils::clamp(globalY + 300.0f, 0.0f, 300.0f) / 350.0f) {
				if (globalY > height) {
					tiles(x, y) = Tile(glm::ivec2(x, y), 0);
				}
				else {
					if (globalY < -40.0f + height2) {
						tiles(x, y) = Tile(glm::ivec2(x, y), 1);
						isEmpty = false;
					}
					else {
						tiles(x, y) = Tile(glm::ivec2(x, y), 2);
						isEmpty = false;
					}
				}
			}
			else {
				tiles(x, y) = Tile(glm::ivec2(x, y), 0);
			}
		}
	}
	meshIsCurrent = false;
}

void WorldChunk::generateVBO() {
	if (isEmpty) return;

	tileMesh.reserve((size_t)chunkSize * chunkSize * sizeof(TileVert)); // reserve half a chunks worth of data idk
	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++) {
			if (tiles(x, y).tileID != 0) {
				unsigned int tID = tiles(x, y).tileID;


				glm::uvec3 pos_tl = glm::uvec3(x, y, 0);
				glm::uvec3 pos_tr = glm::uvec3(x + 1, y, 0);
				glm::uvec3 pos_bl = glm::uvec3(x, y + 1, 0);
				glm::uvec3 pos_br = glm::uvec3(x + 1, y + 1, 0);


				tileMesh.pushVertices({ // push all the calculated tile vertices
					TileVert(pos_tl.x, pos_tl.y, pos_tl.z, // Position attributes
					tID),           // ID Attribute
					TileVert(pos_bl.x, pos_bl.y, pos_bl.z,
					tID),
					TileVert(pos_tr.x, pos_tr.y, pos_tr.z,
					tID),
					TileVert(pos_bl.x, pos_bl.y, pos_bl.z,
					tID),
					TileVert(pos_tr.x, pos_tr.y, pos_tr.z,
					tID),
					TileVert(pos_br.x, pos_br.y, pos_br.z,
					tID)
					});
			}
		}
	}

	tileMesh.genVBO();
	meshIsCurrent = true;
}

unsigned int WorldChunk::getVBOSize() {
	return (uint16_t) tileMesh.getTotalVBOSize();
}

Tile* WorldChunk::getTiles() {
	return tiles.getData().data();
}

void WorldChunk::setChunkTile(glm::ivec2 p_chunkCoordinates, unsigned int p_tileID) {
	tiles(p_chunkCoordinates.x, p_chunkCoordinates.y) = Tile(p_chunkCoordinates, p_tileID);
	return;
}

void WorldChunk::remove() {
	tileMesh.remove();
}

void WorldChunk::draw(DrawSurface& p_target, DrawStates& p_drawStates)
{
	DrawStates newStates = DrawStates(p_drawStates);
	
	newStates.setTransform(p_drawStates.transform * transform);

	p_target.draw(tileMesh, GL_TRIANGLES, newStates);
}
