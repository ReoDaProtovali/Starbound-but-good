#include "WorldGenerator.hpp"
#include <algorithm>
#include <set>
#include "util/utils.hpp"
WorldGenerator::WorldGenerator() {
	initTestSettings();
}

void WorldGenerator::initTestSettings()
{
	m_settings.heightmap.shaderName = "perlin1d";
	m_settings.heightmap.strength = 20.f;

	gen::GeneratorSettings2D_4Layer testHollow;
	testHollow.shaderName = "cavern";
	testHollow.cutoff1 = 0.8f;
	testHollow.cutoff2 = 0.6f;
	testHollow.cutoff3 = 0.4f;
	testHollow.cutoff4 = 0.2f;

	m_settings.testBiome.hollowGenerators.push_back(testHollow);

	// this vector will contain every generation criteria that dirt can spawn in
	auto& perlinRanges = m_settings.testBiome.materialGenerators["perlin"];
	// Only one criteria, when the perlin noise is between 0.5f and 1.0f.
	perlinRanges.emplace_back(gen::MaterialRange{ "vanilla:dirt", 0.3f, 1.0f });
	perlinRanges.emplace_back(gen::MaterialRange{ "vanilla:stone", 0.0f, 0.7f });

}

void WorldGenerator::loadNoiseValuesAtChunk(int32_t p_chunkX, int32_t p_chunkY) {
	for (auto& hollowGenerator : m_settings.testBiome.hollowGenerators) {
		m_noiseMap.genTilesNeighboringChunk(p_chunkX, p_chunkY, hollowGenerator.shaderName);
	}
	for (auto& materialGenerator : m_settings.testBiome.materialGenerators) {
		m_noiseMap.genTilesNeighboringChunk(p_chunkX, p_chunkY, materialGenerator.first);
	}
}
Array3D<Tile> WorldGenerator::generateChunk(int32_t p_chunkX, int32_t p_chunkY, bool& wasEmpty)
{
	// yeah i'm going to need to do this differently for speed later
	wasEmpty = true;
	std::unordered_map<std::string, float> distances;

	Array3D<Tile> out{ CHUNKSIZE, CHUNKSIZE, CHUNKDEPTH };

	float globalChunkX = p_chunkX * CHUNKSIZE;
	float globalChunkY = p_chunkY * CHUNKSIZE;

	for (int z = 0; z < CHUNKDEPTH; z++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int x = 0; x < CHUNKSIZE; x++) {
				// get all generator outputs and compute tile distances
				for (auto& materialGenerator : m_settings.testBiome.materialGenerators) {

					for (gen::MaterialRange& range : materialGenerator.second) {
						distances[range.materialName] = 0.f;
					}

					float noiseMapOutput = m_noiseMap.getPixel(globalChunkX + x, globalChunkY - y, materialGenerator.first).r;
					// to compute the distance, we need the range the material can generate in as well as the values
					for (gen::MaterialRange& range : materialGenerator.second) {
						// invalidate if tile is not qualified for any one range
						if (!range.within(noiseMapOutput) || distances[range.materialName] == 999.f) {
							distances[range.materialName] = 999.f;
							continue;
						}
						// square distance formula stuff
						distances[range.materialName] += noiseMapOutput * noiseMapOutput - 2 * range.center() * noiseMapOutput + range.center() * range.center();
					}
				}
				std::pair<std::string, float> record = { "NULL", 999.f };
				for (auto& dist : distances) {
					if (dist.second < record.second) record = dist;
				}

				if (record.second != 999.f) {
					out(x, y, z) = Tile(x, y, res.getTileInfo(record.first).value().get().spriteIndex);
					wasEmpty = false;
				}
				else {
					out(x, y, z) = Tile(x, y, 0);
				}
			}

		}
	}

	return out;
}
