#include "WorldGenerator.hpp"
#include <algorithm>
#include <set>
#include "util/utils.hpp"
WorldGenerator::WorldGenerator() {
	initTestSettings();
}

void WorldGenerator::initTestSettings()
{

}

Array3D<Tile> WorldGenerator::generateChunk(int32_t p_chunkX, int32_t p_chunkY, bool& wasEmpty)
{
	Array3D<Tile> out{CHUNKSIZE, CHUNKSIZE, CHUNKDEPTH};

	gen::Layer test = {};

	for (int z = 0; z < CHUNKDEPTH; z++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int x = 0; x < CHUNKSIZE; x++) {
				out(x, y, z) = test.getTile(p_chunkX * CHUNKSIZE + x, p_chunkY * CHUNKSIZE - y);
			}
		}
	}

	wasEmpty = false;
	return out;
}

void WorldGenerator::clearNoisemap()
{
	m_noiseMap.clear();
}

void gen::Layer::addTileType(std::string_view p_tileID, float p_value = 0.f)
{
	switch (mode) {
	case LayerMode::BASIC:
		if (tilePalette.size() > 1) WARNING_LOG("Adding additional tile types to a generator layer in basic mode. Old ones will be replaced.");
		tilePalette.clear();
		tilePalette.emplace_back(p_tileID, p_value);

		break;

	case LayerMode::PALETTED:
		tilePalette.emplace_back(p_tileID, p_value);

		break;
	case LayerMode::EXCLUSION:
		WARNING_LOG("Trying to add a tile type to a generator layer in exclusion mode. Will be ignored.");
		break;
	}
}

void gen::Layer::setExclusionThreshold(float p_threshold)
{
	if (mode != LayerMode::EXCLUSION) ERROR_LOG("Setting generator layer exclusion threshold while not in exclusion mode.");
}

Tile gen::Layer::getTile(int32_t p_worldX, int32_t p_worldY)
{
	switch (mode) {
	case LayerMode::BASIC:
		break;
	case LayerMode::PALETTED:
		break;
	case LayerMode::EXCLUSION:
		break;

	}

}
void gen::Layer::setMode(LayerMode p_mode)
{
}

void WorldGenerator::loadNoiseValuesAtChunk(int32_t p_chunkX, int32_t p_chunkY) {

	//m_noiseMap.genTilesNeighboringChunk(p_chunkX, p_chunkY, m_settings.heightmap.shaderName, m_settings.heightmap.seed);
	//for (auto& hollowGenerator : m_settings.testBiome.hollowGenerators) {
	//	m_noiseMap.genTilesNeighboringChunk(p_chunkX, p_chunkY, hollowGenerator.shaderName, hollowGenerator.seed);
	//}
	//for (auto& materialGenerator : m_settings.testBiome.materialGenerators) {
	//	m_noiseMap.genTilesNeighboringChunk(p_chunkX, p_chunkY, materialGenerator.first.shaderName, materialGenerator.first.seed);
	//}
}