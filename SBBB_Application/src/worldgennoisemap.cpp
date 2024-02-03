#include "WorldGenNoisemap.hpp"
#include "GameConstants.hpp"
WorldGenNoisemap::WorldGenNoisemap()
{
	m_square.addFloatAttrib(2);
	m_square.addFloatAttrib(2);
	m_square.pushVertices({
		-1.f, -1.f, 0.f, 0.f,
		1.f, -1.f, 1.f, 0.f,
		-1.f, 1.f, 0.f, 1.f,
		1.f, 1.f, 1.f, 1.f
		});
	m_square.pushIndices({
		0, 1, 2, 2, 1, 3
		});
	m_square.pushIBOToGPU();
	m_square.pushVBOToGPU();
	m_square.clean();

	m_FBO.useDepth(false);
	m_FBO.setDimensions(glm::uvec2(NOISEMAP_TILE_SIZE));


}

glm::vec4 WorldGenNoisemap::getPixel(int32_t p_worldPosX, int32_t p_worldPosY, const std::string& p_generatorName, int p_seed) 
{
	glm::ivec2 tilePos = globalPosToTilePos(p_worldPosX, p_worldPosY);

	std::vector<NoiseTile>& data = m_map[tilePos];
	
	for (auto& tile : data) {
		if (p_generatorName == tile.generator && p_seed == tile.m_seed) {
			uint16_t x = utils::modUnsigned(p_worldPosX, NOISEMAP_TILE_SIZE);
			uint16_t y = utils::modUnsigned(p_worldPosY, NOISEMAP_TILE_SIZE);
			return glm::vec4(
				tile.data(x * 4 + 0, y) / 255.0f,
				tile.data(x * 4 + 1, y) / 255.0f,
				tile.data(x * 4 + 2, y) / 255.0f,
				tile.data(x * 4 + 3, y) / 255.0f
			);
		}
	}
	throw std::exception("Tried to get pixel from non-generated region.");
	return glm::vec4(0);
}

glm::ivec2 WorldGenNoisemap::globalPosToTilePos(int32_t p_worldPosX, int32_t p_worldPosY) 
{
	//return glm::ivec2(p_worldPosX / NOISEMAP_TILE_SIZE + (p_worldPosX < 0 ? -1 : 0), p_worldPosY / NOISEMAP_TILE_SIZE + (p_worldPosY < 0 ? -1 : 0));
	return glm::ivec2(utils::gridFloor(p_worldPosX, NOISEMAP_TILE_SIZE), utils::gridFloor(p_worldPosY, NOISEMAP_TILE_SIZE));
}

void WorldGenNoisemap::genTile(int32_t p_mapX, int32_t p_mapY, const std::string& p_generatorName, int p_seed)
{
	if (m_map.find(glm::ivec2(p_mapX, p_mapY)) != m_map.end()) {
		auto& currentVec = m_map[glm::ivec2(p_mapX, p_mapY)];
		for (auto& it : currentVec) {
			if (p_generatorName == it.generator && p_seed == it.m_seed) {
				return;
			}
		}
	};
#ifndef DISABLE_DEBUG_STATS
	globals.debug.noisemapTileCount++;
#endif

	auto& currentVec = m_map[glm::ivec2(p_mapX, p_mapY)];

	currentVec.emplace_back(p_generatorName, StaticArray2D<uint8_t>(), p_seed);
	//LOAD_LOG("Loaded '" << p_generatorName << "' noisemap tile at " << p_mapX << ", " << p_mapY << " with seed " << p_seed);
	Shader& generatorShader = res.getGeneratorShader(p_generatorName);

	DrawStates d;
	BlendMode noiseBlend;
	noiseBlend.disable();
	d.setBlendMode(noiseBlend);

	d.attachShader(&generatorShader);

	// Location 2 is the world position uniform
	generatorShader.setVec2Uniform(generatorShader.getUniformLoc("WorldPos"), glm::vec2(p_mapX, p_mapY));
	generatorShader.setIntUniform(generatorShader.getUniformLoc("Seed"), p_seed);
	m_FBO.bind();
	m_FBO.draw(m_square, GL_TRIANGLES, d);

	currentVec[currentVec.size() - 1].data.clear();
	m_FBO.getPixels(0, 4, currentVec[currentVec.size() - 1].data);
	currentVec[currentVec.size() - 1].generated = true;
}

void WorldGenNoisemap::genTilesNeighboringChunk(int p_chunkX, int p_chunkY, const std::string& p_generatorName, int p_seed)
{
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			glm::ivec2 tilePos = globalPosToTilePos(p_chunkX * CHUNKSIZE + CHUNKSIZE * j, p_chunkY * CHUNKSIZE + CHUNKSIZE * i);
			genTile(tilePos.x, tilePos.y, p_generatorName, p_seed);
		}
	}
}


void WorldGenNoisemap::clear() {
	if (m_map.size() > 0) {
#ifndef DISABLE_DEBUG_STATS
		globals.debug.noisemapTileCount = 0;
#endif
		m_map.clear();
	}
}