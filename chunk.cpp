#include "Chunk.hpp"
#include "glm/glm.hpp"
#include "GameConstants.hpp"

WorldChunk::WorldChunk(glm::ivec2 p_worldPos, int p_worldID) :worldPos(p_worldPos), worldID(p_worldID), invalid(false) {
	tiles = Array2D<Tile>(chunkSize, chunkSize);
	glGenVertexArrays(1, &VAO);
	
	noiseGenerator.SetOctaveCount(10);
	noiseGenerator.SetPersistence(0.54);
}
void WorldChunk::fillRandom() {
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
	vboIsCurrent = false;
}
void WorldChunk::worldGenerate(glm::ivec2 p_chunkPos) {
	float surfaceLevel = 10.0 * chunkSize;
	float globalX;
	float globalY; // tile positions
	float height;
	float height2;
	float caveLayer1;
	float caveLayer2;

	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++) {
			globalX = p_chunkPos.x * chunkSize + x;
			globalY = p_chunkPos.y * chunkSize - y - surfaceLevel;
			// detail level for the surface noise
			noiseGenerator.SetOctaveCount(7);
			// surface level
			height = noiseGenerator.GetValue(globalX / 200.0f, 214.2, 2.0) * 40.0;
			// The dirt level
			// first noise is a scaled down version of the surface line            second noise is to make a dither effect between the layers
			height2 = noiseGenerator.GetValue(globalX / 200.0f, 214.2, 2.0) * 20.0 - (5.0f * noiseGenerator.GetValue(globalX / 1.2f, globalY / 1.2f, 2.0));
			float scale1 = 1 / 10.0;
			float scale2 = 1 / 1.0;
			noiseGenerator.SetOctaveCount(2);
			caveLayer1 = noiseGenerator.GetValue((double)globalX * scale1, (double)globalY * scale1, 2.0) / 2.0;
			noiseGenerator.SetOctaveCount(1);
			caveLayer2 = noiseGenerator.GetValue(((double)globalX * scale1 * 0.7) + caveLayer1 * scale2, ((double)globalY * scale1 * 1.0) + caveLayer1 * scale2, 40.0f);
			//std::cout << utils::clamp(globalY + surfaceLevel, 0.0f, 10.0f) / 10.0f << std::endl;
			if (caveLayer2 < 0.05f + (utils::clamp(globalY - height + 64.0f, 0.0f, 10.0f) / 10.0f) + utils::clamp(globalY + 300.0f, 0.0f, 300.0f) / 350.0f) {
				if (globalY > height) {
					tiles(x, y) = Tile(glm::ivec2(x, y), 0);
				}
				else {
					if (globalY < -40.0 + height2) {
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
	vboIsCurrent = false;
}
GLuint WorldChunk::generateVBO(SpriteSheet& p_spriteSheet) {
	glBindVertexArray(VAO);

	verts.reserve((size_t)chunkSize * chunkSize * 6);

	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++) {
			if (tiles(x, y).tileID != 0) {
				unsigned int tID = tiles(x, y).tileID;
				float x_f = (float)x;
				float y_f = (float)-y; // to make the model origin top left, I set the y to be negative
				float imprecisionCorrection = 0.0005f; // removes occasional black lines that show up between tiles

				float m_Off = 0.0f; // set this to chunkSize / 2.0 if you want the chunk origin relative to the center
				glm::vec3 pos_tl = glm::vec3(x_f - m_Off, y_f - m_Off, 0.0f);
				glm::vec3 pos_tr = glm::vec3(x_f + 1.0f - m_Off + imprecisionCorrection, y_f - m_Off, 0.0f);
				glm::vec3 pos_bl = glm::vec3(x_f - m_Off, y_f + 1.0f - m_Off + imprecisionCorrection, 0.0f);
				glm::vec3 pos_br = glm::vec3(x_f + 1.0f - m_Off + imprecisionCorrection, y_f + 1.0f - m_Off + imprecisionCorrection, 0.0f);
				glm::vec2 tex_tl = p_spriteSheet.getTexCoords(tID, Corner::TOP_LEFT) + imprecisionCorrection;
				glm::vec2 tex_tr = p_spriteSheet.getTexCoords(tID, Corner::TOP_RIGHT) - glm::vec2(imprecisionCorrection, -imprecisionCorrection);
				glm::vec2 tex_bl = p_spriteSheet.getTexCoords(tID, Corner::BOTTOM_LEFT) - glm::vec2(-imprecisionCorrection, imprecisionCorrection);
				glm::vec2 tex_br = p_spriteSheet.getTexCoords(tID, Corner::BOTTOM_RIGHT) - imprecisionCorrection;

				Vertex v1 = Vertex(pos_tl, tex_tl); // First triangle
				Vertex v2 = Vertex(pos_bl, tex_bl);
				Vertex v3 = Vertex(pos_tr, tex_tr);
				Vertex v4 = Vertex(pos_bl, tex_bl); // Second triangle
				Vertex v5 = Vertex(pos_tr, tex_tr);
				Vertex v6 = Vertex(pos_br, tex_br);

				WorldChunk::verts.insert(WorldChunk::verts.end(), { v1, v2, v3, v4, v5, v6 });
			}
		}
	}
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(attrib_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(attrib_texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(attrib_position);
	glEnableVertexAttribArray(attrib_texCoord);
	vboIsCurrent = true;
	return VBO;
}

int WorldChunk::getVBOSize() {
	return (int)verts.size();
}

Tile* WorldChunk::getTiles() {
	return tiles.getData();
}
void WorldChunk::setChunkTile(glm::ivec2 p_chunkCoordinates) {
	return;
}