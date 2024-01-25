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
		PALETTED
	};
	// Similar to color stops, a tile stop determines how the tile is picked from the palette within palette mode.
	struct TileStop {
		std::string tileID;
		// keep from 0 to 1; gradient stop position
		float position = 0.f;
	};
	class Layer {
	public:
		std::string shaderName;
		
		Tile getTile(int32_t p_worldX, int32_t p_worldY);

	private:
		LayerMode mode;
		std::vector<TileStop> tilePalette;
	};

	class LayerDepthGroup {
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
	void clearNoisemap();
private:
	
	WorldGenNoisemap m_noiseMap;
	ResourceManager& res = ResourceManager::Get();
};

