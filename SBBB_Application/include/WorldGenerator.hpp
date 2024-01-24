#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "WorldGenNoisemap.hpp"
#include "ResourceManager.hpp"
#include "Tile.hpp"
#include "util/Array3D.hpp"



class WorldGenerator
{
public:
	WorldGenerator();
	void initTestSettings();
	Array3D<Tile> generateChunk(int32_t p_x, int32_t p_y, bool& wasEmpty);
	void clearNoisemap();
private:
	
	gen::WorldSettings m_settings;
	WorldGenNoisemap m_noiseMap;
	ResourceManager& res = ResourceManager::Get();
};

