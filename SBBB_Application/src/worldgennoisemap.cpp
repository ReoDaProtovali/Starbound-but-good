#include "WorldGenNoisemap.hpp"
#include "ResourceManager.hpp"
WorldGenNoisemap::WorldGenNoisemap()
{
	m_square.addFloatAttrib(2);
	m_square.pushVertices({
		-1.f, -1.f,
		1.f, -1.f,
		-1.f, 1.f,
		1.f, 1.f
		});
	m_square.pushIndices({
		0, 1, 2, 2, 1, 3
		});
	m_square.genIBO();
	m_square.genVBO();

	// wont work if it's not disabled for some reason
	m_FBO.useDepth(false);
	m_FBO.setDimensions(glm::uvec2(NOISEMAP_TILE_SIZE));

	//genTile(0, -1, "perlin");
	//auto x = getPixel(50, -513, "perlin");

}

glm::vec4 WorldGenNoisemap::getPixel(int32_t p_worldPosX, int32_t p_worldPosY, const std::string& p_generatorName) 
{

	glm::ivec2 tilePos = globalPosToTilePos(p_worldPosX, p_worldPosY);
	genTile(tilePos.x, tilePos.y, p_generatorName);
	std::vector<NoiseTile>& data = m_map[tilePos];
	for (auto& tile : data) {
		if (p_generatorName == tile.generator) {
			uint16_t x = utils::fwrapUnsigned(-(float)p_worldPosX, (float)NOISEMAP_TILE_SIZE - 1.f) * 4;
			uint16_t y = utils::fwrapUnsigned((float)p_worldPosY, (float)NOISEMAP_TILE_SIZE - 1.f);
			return glm::vec4(
				tile.data(x + 0, y) / 255.0f,
				tile.data(x + 1, y) / 255.0f,
				tile.data(x + 2, y) / 255.0f,
				tile.data(x + 3, y) / 255.0f
			);
		}
	}
	throw std::exception("unreachable");
	return glm::vec4(0);
}

glm::ivec2 WorldGenNoisemap::globalPosToTilePos(int32_t p_worldPosX, int32_t p_worldPosY) 
{
	return glm::ivec2(p_worldPosX / NOISEMAP_TILE_SIZE, p_worldPosY / NOISEMAP_TILE_SIZE);
}

void WorldGenNoisemap::genTile(int32_t p_mapX, int32_t p_mapY, const std::string& p_generatorName)
{
	if (m_map.find(glm::ivec2(p_mapX, p_mapY)) != m_map.end()) {
		auto& currentVec = m_map[glm::ivec2(p_mapX, p_mapY)];
		for (auto& it : currentVec) {
			if (p_generatorName == it.generator) {
				return;
			}
		}
	};
	auto& currentVec = m_map[glm::ivec2(p_mapX, p_mapY)];

	currentVec.emplace_back(p_generatorName, Array2D<uint8_t>());

	ResourceManager& res = ResourceManager::Get();
	Shader& generatorShader = res.getGeneratorShader(p_generatorName);
	DrawStates d;
	d.attachShader(&generatorShader);

	// Location 0 is the world position uniform
	generatorShader.setVec2Uniform(0, glm::vec2(p_mapX, p_mapY));

	m_FBO.bind();
	m_FBO.draw(m_square, GL_TRIANGLES, d);

	currentVec[currentVec.size() - 1].data.clear();
	m_FBO.getPixels(0, 4, currentVec[currentVec.size() - 1].data);

}