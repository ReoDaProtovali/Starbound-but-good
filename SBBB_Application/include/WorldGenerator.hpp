#pragma once

#include <string>
#include <vector>
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
		std::string shaderName;
		uint64_t seed;

		// only cares about the first addition if the mode is basic
		// in basic mode, value can be ignored
		// in palleted mode, value corresponds to stop position
		void addTileType(std::string_view p_tileID, float p_value);
		void setExclusionThreshold(float p_threshold);

		Tile getTile(int32_t p_worldX, int32_t p_worldY);
		void setMode(LayerMode p_mode);

	private:
		LayerMode mode = LayerMode::BASIC;
		std::vector<TileStop> tilePalette;
	};

	class LayerGroup {
		Layer z_levels[CHUNKDEPTH] = {};
	};

	class Stack {

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
	ResourceManager& res = ResourceManager::Get();
};

