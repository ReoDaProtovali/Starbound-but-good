#include "Chunk.hpp"
#include "glm/glm.hpp"
#include "GameConstants.hpp"

WorldChunk::WorldChunk(glm::ivec2 p_worldPos, int p_worldID) :worldPos(p_worldPos), worldID(p_worldID) {
	for (int y = 0; y < chunkSize; y++) {
		tiles[y] = new Tile[chunkSize];
	}
}
void WorldChunk::cleanUp() {
	for (int y = 1; y < chunkSize; y++) {
		//std::cout << /*typeid(tiles[y]).name()*/"THIS LINE OF CODE WAS EXECUTED" << std::endl;
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
	return;
}
GLuint WorldChunk::generateVBO(SpriteSheet p_spriteSheet) {

	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++) {
			if (WorldChunk::tiles[y][x].tileID != 0) {
				unsigned int tID = WorldChunk::tiles[y][x].tileID;
				glm::vec3 pos_tl = glm::vec3((float)x, (float)y, 0.0f);
				glm::vec3 pos_tr = glm::vec3((float)x + 1.0f, (float)y, 0.0f);
				glm::vec3 pos_bl = glm::vec3((float)x, (float)y + 1.0f, 0.0f);
				glm::vec3 pos_br = glm::vec3((float)x + 1.0f, (float)y + 1.0f, 0.0f);
				glm::vec2 tex_tl = p_spriteSheet.getTexCoords(tID, Corner::TOP_LEFT);
				glm::vec2 tex_tr = p_spriteSheet.getTexCoords(tID, Corner::TOP_RIGHT);
				glm::vec2 tex_bl = p_spriteSheet.getTexCoords(tID, Corner::BOTTOM_LEFT);
				glm::vec2 tex_br = p_spriteSheet.getTexCoords(tID, Corner::BOTTOM_RIGHT);

				Vertex v1 = Vertex(pos_tl, tex_tl); // First triangle
				Vertex v2 = Vertex(pos_bl, tex_bl);
				Vertex v3 = Vertex(pos_tr, tex_tr);
				Vertex v4 = Vertex(pos_bl, tex_bl); // Second triangle
				Vertex v5 = Vertex(pos_tr, tex_tr);
				Vertex v6 = Vertex(pos_br, tex_br);
				//std::cout << v1.pos.x << " " << v1.pos.y << " " << v1.texCoord.x << " " << v1.texCoord.y << std::endl;
				//std::cout << v2.pos.x << " " << v2.pos.y << " " << v2.texCoord.x << " " << v2.texCoord.y << std::endl;
				//std::cout << v3.pos.x << " " << v3.pos.y << " " << v3.texCoord.x << " " << v3.texCoord.y << std::endl;
				//std::cout << v4.pos.x << " " << v4.pos.y << " " << v4.texCoord.x << " " << v4.texCoord.y << std::endl;
				//std::cout << v5.pos.x << " " << v5.pos.y << " " << v5.texCoord.x << " " << v5.texCoord.y << std::endl;
				//std::cout << v6.pos.x << " " << v6.pos.y << " " << v6.texCoord.x << " " << v6.texCoord.y << std::endl;

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
	return VBO;
}

Tile** WorldChunk::getTiles() {
	return tiles;
}
void WorldChunk::setChunkTile(glm::ivec2 p_chunkCoordinates) {
	return;
}