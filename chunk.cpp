#include "Chunk.hpp"
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>

WorldChunk::WorldChunk(Vector2i p_worldPos, int p_worldID) :worldPos(p_worldPos), worldID(p_worldID) {
	for (int y = 0; y < 128; y++) {
		tiles[y] = new Tile[128];
	}
}
void WorldChunk::cleanUp() {
	for (int y = 1; y < 128; y++) {
		//std::cout << /*typeid(tiles[y]).name()*/"THIS LINE OF CODE WAS EXECUTED" << std::endl;
		delete tiles[y];
	}
	delete tiles;
}
void WorldChunk::fillRandom() {
	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			if (rand() % 10 > 5) {
				tiles[y][x] = Tile(Vector2i(x, y), 0);
			}
			else {
				tiles[y][x] = Tile(Vector2i(x, y), 1);
			}
		}
	}
	return;
}
void WorldChunk::generateVBO(SpriteSheet p_spriteSheet, unsigned int p_vao) {
	glGenVertexArrays(1, &p_vao);

	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			if (WorldChunk::tiles[y][x].tileID != 0) {
				WorldChunk::verts.insert(WorldChunk::verts.end(), {});
			}
		}
	}

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindVertexArray(p_vao);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts.front(), GL_STATIC_DRAW);


	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(2 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

Tile** WorldChunk::getTiles() {
	return tiles;
}
void WorldChunk::setChunkTile(Vector2i p_chunkCoordinates) {
	return;
}