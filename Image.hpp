#pragma once
#include "GameConstants.h"
#include "MathUtils.hpp"
struct Image
{
	Image() : data(nullptr), dimensions(Vector2i()), ID(TextureID::NO_TEXTURE) {};
	Image(unsigned char* p_data, Vector2i p_dimensions, TextureID p_ID) : data(p_data), dimensions(p_dimensions), ID(p_ID) {};
	unsigned char* data;
	Vector2i dimensions;
	TextureID ID;
};

