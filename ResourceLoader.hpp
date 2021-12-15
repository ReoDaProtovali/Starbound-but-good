#pragma once
#include <vector>
#include <SDL.h>
#include "MathUtils.hpp"
#include <iostream>
class ResourceLoader
{
public:
	ResourceLoader() {};
	Uint16 load(const char* p_filepath);
	unsigned char* getTex(Uint16 p_ID);
	Vector2i getDimensions(Uint16 p_ID);
private:
	Uint16 currentID = 1;
	std::vector<unsigned char*> textures;
	std::vector<Vector2i> dimensions;
};

