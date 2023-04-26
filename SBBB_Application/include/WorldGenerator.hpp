#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "WorldGenNoisemap.hpp"
#include "ResourceManager.hpp"
#include "Tile.hpp"
#include "util/Array3D.hpp"

namespace gen {
	// 1d generator, used to set surface level
	// i don't know what to name this
	struct GeneratorSettings1D_1Layer {
		GeneratorSettings1D_1Layer() 
			: shaderName("NULL"), strength(0.f) {};
		GeneratorSettings1D_1Layer(const std::string& p_shaderName, int p_seed)
			: shaderName(p_shaderName), seed(p_seed), strength(0.f) {};
		GeneratorSettings1D_1Layer& operator=(const GeneratorSettings1D_1Layer& other) {
			seed = other.seed;
			shaderName = other.shaderName;
			strength = other.strength;
			return *this;
		}
		std::string shaderName;
		int32_t seed = 0; // UNIMPLEMENTED
		// controls how extreme the generator influences height
		float strength;
		bool operator==(const GeneratorSettings1D_1Layer& rhs) const {
			return this->shaderName == rhs.shaderName && this->seed == rhs.seed;
		}
	};
	// 2d generator, determines empty regions of the world
	struct GeneratorSettings2D_4Layer {
		GeneratorSettings2D_4Layer() 
			: shaderName("NULL") {};
		GeneratorSettings2D_4Layer(const std::string& p_shaderName, int p_seed)
			: shaderName(p_shaderName), seed(p_seed) {};
		GeneratorSettings2D_4Layer& operator=(const GeneratorSettings2D_4Layer& other) {
			seed = other.seed;
			shaderName = other.shaderName;
			memcpy(cutoffs, other.cutoffs, sizeof(cutoffs));
			return *this;
		}
		std::string shaderName;
		int32_t seed = 0;
		// determines the greyscale cutoff point for how sparse or solid the generator is
		// static array with one value per each world layer, might make it more controllable later
		float cutoffs[CHUNKDEPTH] = {0};
		bool operator==(const GeneratorSettings2D_4Layer& rhs) const {
			return this->shaderName == rhs.shaderName && this->seed == rhs.seed;
		}
	};
	struct GeneratorSettingsHash {
		size_t operator()(const GeneratorSettings1D_1Layer& value) const { return std::hash<std::string>{}(value.shaderName) + std::hash<int>{}(value.seed); }
		size_t operator()(const GeneratorSettings2D_4Layer& value) const { return std::hash<std::string>{}(value.shaderName) + std::hash<int>{}(value.seed); }
	};

	// Determines which ranges a certain material is able to generate, contained within a vector associated with a particular shader
	struct MaterialRange {
		std::string materialName;
		float min;
		float max;
		// if it is exclusive, then the generator *must* be within the range to be a canidate.
		bool exclusive = true;
		// specifies that narrower ranges get priority
		// weird way of doing it
		inline float variance() { return ((min - center()) * (min - center()) + (max - center()) * (max - center())) / (max - min); }
		inline float center() { return (min + max) / 2.f; }
		inline bool within(float value) { return value > min && value < max; }
	};


	
	struct BiomeSettings {
		// Determines empty tiles
		std::vector<GeneratorSettings2D_4Layer> hollowGenerators;
		// Maps between shader names and lists of valid generator ranges
		std::unordered_map<GeneratorSettings2D_4Layer, std::vector<MaterialRange>, GeneratorSettingsHash> materialGenerators;
		
	};

	struct WorldSettings {
		GeneratorSettings1D_1Layer heightmap;
		// TODO :: biomes 
		BiomeSettings testBiome;
	};
}


class WorldGenerator
{
public:
	WorldGenerator();
	void initTestSettings();
	Array3D<Tile> generateChunk(int32_t p_x, int32_t p_y, bool& wasEmpty);
	void loadNoiseValuesAtChunk(int32_t p_x, int32_t p_y);
	void clearNoisemap();
private:
	
	gen::WorldSettings m_settings;
	WorldGenNoisemap m_noiseMap;
	ResourceManager& res = ResourceManager::Get();
};

