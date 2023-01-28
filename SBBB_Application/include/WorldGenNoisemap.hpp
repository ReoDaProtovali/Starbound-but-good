#pragma once
#include "Framework/Graphics/FrameBuffer.hpp"
#include "Framework/Graphics/Shader.hpp"
#include "util/Array2D.hpp"
#include "util/ext/glm/vec2.hpp"
#include <unordered_map>
#include <string>

#define NOISEMAP_TILE_SIZE 512

struct ivec2Hash {

	size_t operator()(const glm::ivec2& p_key) const {
		return (p_key.y << 16) ^ p_key.x;
	}
};

struct NoiseTile {
	std::string generator;
	StaticArray2D<uint8_t> data;
};

class WorldGenNoisemap
{
public:
	WorldGenNoisemap();

	// Gets a single pixel from the noisemap at a given world position. Generates a noise texture at that position if it doesn't exist.
	// Because this class stores multiple generators, specify the name with p_generatorName
	glm::vec4 getPixel(int32_t p_worldPosX, int32_t p_worldPosY, const std::string& p_generatorName);

	// Renders a noise texture for a given position
	// tile in this case refers to a noisemap tile
	void genTile(int32_t p_mapX, int32_t p_mapY, const std::string& p_generatorName);

private:
	glm::ivec2 globalPosToTilePos(int32_t p_worldPosX, int32_t p_worldPosY);
	FrameBuffer m_FBO;
	Mesh<GLfloat> m_square;

	std::unordered_map<glm::ivec2, std::vector<NoiseTile>, ivec2Hash> m_map;
};

