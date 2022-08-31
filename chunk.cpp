#include "Chunk.hpp"
#include "glm/glm.hpp"
#include "GameConstants.hpp"

WorldChunk::WorldChunk(glm::ivec2 p_worldPos, int p_worldID) :worldPos(p_worldPos), worldID(p_worldID), invalid(false) {
	tiles = Array2D<Tile>(chunkSize, chunkSize, Tile());

	tileMesh.addFloatAttrib(3); // The position attribute, Three floats
	tileMesh.addFloatAttrib(2); // The texcoord attribute, two floats

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
void WorldChunk::worldGenerate(glm::ivec2 p_chunkPos) {
	isEmpty = false;

	float surfaceLevel = 10.0f * chunkSize;
	float globalX;
	float globalY; // tile positions
	float height;
	float height2;
	float caveLayer1;
	float caveLayer2;

	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++) {
			globalX = (float)(p_chunkPos.x * chunkSize + x);
			globalY = (float)(p_chunkPos.y * chunkSize - y) - surfaceLevel;
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
					}
					else {
						tiles(x, y) = Tile(glm::ivec2(x, y), 2);
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

void WorldChunk::generateVBO(SpriteSheet& p_spriteSheet) {
	if (isEmpty) return;

	tileMesh.reserve((size_t)chunkSize * chunkSize * 3); // reserve half a chunks worth of data idk
	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++) {
			if (tiles(x, y).tileID != 0) {
				unsigned int tID = tiles(x, y).tileID;
				p_spriteSheet.setCurrentSprite(tID);

				float x_f = (float)x;
				float y_f = (float)-y; // to make the model origin top left, I set the y to be negative
				float imprecisionCorrection = 0.0005f; // removes occasional black lines that show up between tiles

				float m_Off = 0.0f; // set this to chunkSize / 2.0 if you want the chunk origin relative to the center
				glm::vec3 pos_tl = glm::vec3(x_f - m_Off, y_f - m_Off, 0.0f);
				glm::vec3 pos_tr = glm::vec3(x_f + 1.0f - m_Off + imprecisionCorrection, y_f - m_Off, 0.0f);
				glm::vec3 pos_bl = glm::vec3(x_f - m_Off, y_f + 1.0f - m_Off + imprecisionCorrection, 0.0f);
				glm::vec3 pos_br = glm::vec3(x_f + 1.0f - m_Off + imprecisionCorrection, y_f + 1.0f - m_Off + imprecisionCorrection, 0.0f);
				glm::vec2 tex_tl = p_spriteSheet.getTexCoords(Corner::TOP_LEFT) + imprecisionCorrection;
				glm::vec2 tex_tr = p_spriteSheet.getTexCoords(Corner::TOP_RIGHT) - glm::vec2(imprecisionCorrection, -imprecisionCorrection);
				glm::vec2 tex_bl = p_spriteSheet.getTexCoords(Corner::BOTTOM_LEFT) - glm::vec2(-imprecisionCorrection, imprecisionCorrection);
				glm::vec2 tex_br = p_spriteSheet.getTexCoords(Corner::BOTTOM_RIGHT) - imprecisionCorrection;

				tileMesh.pushVertices({ // push all the calculated tile vertices
					TestVert(pos_tl.x, pos_tl.y, pos_tl.z, // Position attributes
					tex_tl.x, tex_tl.y),           // Texcoord attributes
					TestVert(pos_bl.x, pos_bl.y, pos_bl.z,
					tex_bl.x, tex_bl.y),
					TestVert(pos_tr.x, pos_tr.y, pos_tr.z,
					tex_tr.x, tex_tr.y),
					TestVert(pos_bl.x, pos_bl.y, pos_bl.z,
					tex_bl.x, tex_bl.y),
					TestVert(pos_tr.x, pos_tr.y, pos_tr.z,
					tex_tr.x, tex_tr.y),
					TestVert(pos_br.x, pos_br.y, pos_br.z,
					tex_br.x, tex_br.y)
					});
			}
		}
	}

	tileMesh.genVBO();
	meshIsCurrent = true;
}

unsigned int WorldChunk::getVBOSize() {
	return tileMesh.getTotalVBOSize();
}

Tile* WorldChunk::getTiles() {
	return tiles.getData().data();
}

void WorldChunk::setChunkTile(glm::ivec2 p_chunkCoordinates) {
	return;
}

void WorldChunk::remove() {
	tileMesh.remove();
}
