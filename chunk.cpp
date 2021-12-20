#include "Chunk.hpp"
#include "glm/glm.hpp"
#include "GameConstants.hpp"

WorldChunk::WorldChunk(glm::ivec2 p_worldPos, int p_worldID) :worldPos(p_worldPos), worldID(p_worldID) {
	tiles = new Tile * [128];
	glGenVertexArrays(1, &VAO);
	for (int y = 0; y < chunkSize; y++) {
		tiles[y] = new Tile[chunkSize];
	}
}
void WorldChunk::cleanUp() {
	for (int y = 1; y < chunkSize; y++) {
		//std::cout << typeid(tiles[y]).name()/*"THIS LINE OF CODE WAS EXECUTED"*/ << std::endl;
		delete tiles[y];
	}
	delete tiles;
}
void WorldChunk::fillRandom() {
	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++) {
			if (rand() % 10 > 5) {
				tiles[y][x] = Tile(glm::ivec2(x, y), 1);
			}
			else {
				tiles[y][x] = Tile(glm::ivec2(x, y), 2);
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
			if (WorldChunk::tiles[y][x].tileID != 0) {
				unsigned int tID = WorldChunk::tiles[y][x].tileID;
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
	return verts.size();
}

Tile** WorldChunk::getTiles() {
	return tiles;
}
void WorldChunk::setChunkTile(glm::ivec2 p_chunkCoordinates) {
	return;
}