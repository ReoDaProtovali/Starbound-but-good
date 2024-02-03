#pragma once

#include <string>
#include <vector>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include "WorldGenNoisemap.hpp"
#include "ResourceManager.hpp"
#include "Tile.hpp"
#include "util/Array3D.hpp"

// everything in this namespace refers to generation primitives
namespace gen {
	enum class LayerMode {
		BASIC,
		PALETTED,
		EXCLUSION
	};
	// Similar to color stops, a tile stop determines how the tile is picked from the palette within palette mode.
	struct TileStop {
		TileStop(std::string_view a, float b) : tileID(a), position(b) {};
		std::string tileID;
		// keep from 0 to 1; gradient stop position
		float position = 0.f;
	};
	class Layer {
	public:
		Layer(std::string_view p_shaderName, int32_t p_seed, uint8_t p_channel = 0) : m_shaderName(p_shaderName), m_seed(p_seed), channel(p_channel) {};
		Layer() : m_shaderName("NULL"), m_seed(0), channel(0) {};
		// only cares about the first addition if the mode is basic
		// in basic mode, p_threshold is a cutoff for noisemap r channel
		// in palleted mode, p_threshold corresponds to stop position
		void addTileType(std::string_view p_tileID, float p_threshold);
		void setExclusionThreshold(float p_threshold);

		Tile getTile(int32_t p_worldX, int32_t p_worldY, WorldGenNoisemap& p_nm);
		void setMode(LayerMode p_mode);
		// determines which color channel it uses from the generator output
		void setChannel(uint8_t channel);
		LayerMode getMode();
		const std::string& getShaderName();
		int32_t getSeed();

	private:
		LayerMode m_mode = LayerMode::BASIC;
		std::vector<TileStop> m_tilePalette;
		std::string m_shaderName;
		// unfortunately, seed is only 32bit for now
		int32_t m_seed;
		uint8_t channel;
	};

	struct LayerGroup {
		void setLayer(size_t p_depth, std::string_view p_shaderName, int p_seed);
		void setAllLevelsShader(std::string_view p_shaderName, int p_seed);
		void addTileToAllLevels(std::string_view p_tileID, float p_threshold);
		void setAllLevelChannels(uint8_t p_channel);
		void setAllLevelModes(LayerMode mode);
		void setAllExclusionThresholds(float threshold);
		void makeSolid(std::string_view p_tileID);
		std::array<Layer, CHUNKDEPTH> z_levels;
	};

	class Stack {
	public:
		void push(LayerGroup& p_group);
		Tile getTile(int32_t p_worldX, int32_t p_worldY, uint32_t p_depth, WorldGenNoisemap& p_nm);
		void loadNoiseValuesAtChunk(int32_t p_chunkX, int32_t p_chunkY, WorldGenNoisemap& p_noisemap);
	private:
		std::vector<LayerGroup> m_stack;
	};
}

class WorldGenerator
{
public:
	WorldGenerator();
	void initTestSettings();
	Array3D<Tile> generateChunk(int32_t p_x, int32_t p_y, bool& wasEmpty);
	void loadNoiseValuesAtChunk(int32_t p_chunkX, int32_t p_chunkY);
	void clearNoisemap();
private:
	
	WorldGenNoisemap m_noiseMap;
	gen::Stack m_generatorStack;
	ResourceManager& res = ResourceManager::Get();
};

