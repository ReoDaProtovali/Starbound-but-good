#include "WorldGenerator.hpp"
#include <algorithm>
#include <set>
#include "util/utils.hpp"
WorldGenerator::WorldGenerator() {
	initTestSettings();
}

void WorldGenerator::initTestSettings()
{
	gen::LayerGroup perlinDirt;
	perlinDirt.setAllLevelsShader("perlin0.5x", 0);
	perlinDirt.setAllLevelChannels(3);
	perlinDirt.addTileToAllLevels("vanilla:dirt", 0.5);

	gen::LayerGroup perlinStone;
	perlinStone.setAllLevelsShader("perlin0.5x", 3);
	perlinStone.setAllLevelChannels(3);
	perlinStone.addTileToAllLevels("vanilla:richstone", 0.6);


	gen::LayerGroup pureStone;
	pureStone.makeSolid("vanilla:stone");

	gen::LayerGroup cavernCarver;
	cavernCarver.setAllLevelsShader("cavern", 0);
	cavernCarver.setAllLevelModes(gen::LayerMode::EXCLUSION);
	cavernCarver.z_levels[0].setExclusionThreshold(1.4);
	cavernCarver.z_levels[1].setExclusionThreshold(0.3);
	cavernCarver.z_levels[2].setExclusionThreshold(0.2);
	cavernCarver.z_levels[3].setExclusionThreshold(0.1);

	gen::LayerGroup heightmapCarver;
	heightmapCarver.setAllLevelsShader("heightcutoff1", 0);
	heightmapCarver.setAllLevelModes(gen::LayerMode::EXCLUSION);
	heightmapCarver.setAllExclusionThresholds(0.5f);

	gen::LayerGroup copeite_cover;
	gen::LayerGroup copeite;
	copeite.setAllLevelsShader("black", 0);
	copeite.setLayer(3, "perlin0.5x", 2);
	copeite.z_levels[3].addTileType("vanilla:copeite", 0.8);

	m_generatorStack.push(pureStone);
	m_generatorStack.push(perlinStone);
	m_generatorStack.push(copeite);
	m_generatorStack.push(perlinDirt);
	m_generatorStack.push(cavernCarver);
	m_generatorStack.push(heightmapCarver);
}

Array3D<Tile> WorldGenerator::generateChunk(int32_t p_chunkX, int32_t p_chunkY, bool& wasEmpty)
{
	Array3D<Tile> out{CHUNKSIZE, CHUNKSIZE, CHUNKDEPTH};

	for (int z = 0; z < CHUNKDEPTH; z++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int x = 0; x < CHUNKSIZE; x++) {
				out(x, y, z) = m_generatorStack.getTile(p_chunkX * CHUNKSIZE + x, p_chunkY * CHUNKSIZE - y, z, m_noiseMap);
				if (out(x, y, z).m_tileID != 0) wasEmpty = false;
			}
		}
	}

	return out;
}

void WorldGenerator::clearNoisemap()
{
	m_noiseMap.clear();
}

void gen::Layer::addTileType(std::string_view p_tileID, float p_threshold = 0.f)
{
	switch (m_mode) {
	case LayerMode::BASIC:
		if (m_tilePalette.size() > 1) WARNING_LOG("Adding additional tile types to a generator layer in basic mode. Old ones will be replaced.");
		m_tilePalette.clear();
		m_tilePalette.emplace_back(p_tileID, p_threshold);

		break;
	case LayerMode::PALETTED:
		m_tilePalette.emplace_back(p_tileID, p_threshold);

		break;
	case LayerMode::EXCLUSION:
		WARNING_LOG("Trying to add a tile type to a generator layer in exclusion mode. Will be ignored.");
		break;
	}
}

void gen::Layer::setExclusionThreshold(float p_threshold)
{
	if (m_mode != LayerMode::EXCLUSION) ERROR_LOG("Setting generator layer exclusion threshold while not in exclusion mode.");
	m_tilePalette.clear();
	m_tilePalette.emplace_back("vanilla:test", p_threshold);
}

Tile gen::Layer::getTile(int32_t p_worldX, int32_t p_worldY, WorldGenNoisemap& p_nm)
{
	if (m_tilePalette.empty()) return Tile();
	ResourceManager& res = ResourceManager::Get();
	glm::vec4 px;

	switch (m_mode) {
	case LayerMode::BASIC:
		px = p_nm.getPixel(p_worldX, p_worldY, m_shaderName, m_seed);
		if (px[channel] > m_tilePalette[0].position) {
			return res.getTileNumericalID(m_tilePalette[0].tileID).value_or(0);
		}
		else {
			return 0;
		}
	case LayerMode::PALETTED:
		break;
	case LayerMode::EXCLUSION:
		px = p_nm.getPixel(p_worldX, p_worldY, m_shaderName, m_seed);
		if (px[channel] > m_tilePalette[0].position) {
			return 1; // just some hardcoded behaviour. 
		}
		break;
	default:
		break;
	}
	return 0;
}
void gen::Layer::setMode(LayerMode p_mode)
{
	m_mode = p_mode;
}

void gen::Layer::setChannel(uint8_t p_channel)
{
	if (p_channel >= 4) {
		ERROR_LOG("Channel out of range. Only RGBA shaders are accepted at max.");
		channel = 0;
		return;
	}
	channel = p_channel;
}

gen::LayerMode gen::Layer::getMode()
{
	return m_mode;
}

const std::string& gen::Layer::getShaderName()
{
	return m_shaderName;
}

int32_t gen::Layer::getSeed()
{
	return m_seed;
}

void WorldGenerator::loadNoiseValuesAtChunk(int32_t p_chunkX, int32_t p_chunkY) {
	m_generatorStack.loadNoiseValuesAtChunk(p_chunkX, p_chunkY, m_noiseMap);
}

void gen::LayerGroup::setLayer(size_t p_depth, std::string_view p_shaderName, int p_seed)
{
	try {
		z_levels.at(p_depth) = Layer(p_shaderName, p_seed);
	}
	catch (std::out_of_range e) {
		ERROR_LOG("Tried to set a generator layer for a depth value that doesn't exist.");
	}

}

void gen::LayerGroup::setAllLevelsShader(std::string_view p_shaderName, int p_seed = 0)
{
	for (auto& l : z_levels) {
		l = Layer(p_shaderName, p_seed);
	}
}

void gen::LayerGroup::addTileToAllLevels(std::string_view p_tileID, float p_threshold)
{
	for (auto& l : z_levels) {
		l.addTileType(p_tileID, p_threshold);
	}
}

void gen::LayerGroup::setAllLevelChannels(uint8_t p_channel)
{
	for (auto& l : z_levels) {
		l.setChannel(p_channel);
	}
}

void gen::LayerGroup::setAllLevelModes(LayerMode mode)
{
	for (auto& l : z_levels) {
		l.setMode(mode);
	}
}

void gen::LayerGroup::setAllExclusionThresholds(float threshold)
{
	for (auto& l : z_levels) {
		l.setExclusionThreshold(threshold);
	}
}

void gen::LayerGroup::makeSolid(std::string_view p_tileID)
{
	setAllLevelsShader("white", 0);
	setAllLevelModes(LayerMode::BASIC);
	addTileToAllLevels(p_tileID, 0.f);
}

// a little simple. oh well
void gen::Stack::push(LayerGroup& p_group)
{
	m_stack.push_back(p_group);
}

Tile gen::Stack::getTile(int32_t p_worldX, int32_t p_worldY, uint32_t p_depth, WorldGenNoisemap& p_nm)
{
	assert(p_depth < CHUNKDEPTH);
	for (int i = m_stack.size() - 1; i >= 0; i--) {
		auto& currentLayer = m_stack[i].z_levels[p_depth];
		Tile layerTile = currentLayer.getTile(p_worldX, p_worldY, p_nm);
		if (currentLayer.getMode() == LayerMode::EXCLUSION && layerTile.m_tileID == 1) {
			return 0;
		}
		if (layerTile.m_tileID != 0) return layerTile;
	}
	return Tile();
}

void gen::Stack::loadNoiseValuesAtChunk(int32_t p_chunkX, int32_t p_chunkY, WorldGenNoisemap& p_noisemap)
{
	// a little intensive but as long as it works
	for (LayerGroup& lg : m_stack) {
		for (Layer& l : lg.z_levels) {
			p_noisemap.genTilesNeighboringChunk(p_chunkX, p_chunkY, l.getShaderName(), l.getSeed());
		}
	}
}
