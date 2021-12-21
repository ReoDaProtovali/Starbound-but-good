#pragma once
#include "GameConstants.hpp"
#include "glm/glm.hpp"

struct Image
{
	Image() : data(nullptr), dimensions(glm::ivec2()), ID(TextureID::NO_TEXTURE) {};
	Image(unsigned char* p_data, glm::ivec2 p_dimensions, TextureID p_ID) : data(p_data), dimensions(p_dimensions), ID(p_ID) {};
	int pixelCount() { return Image::dimensions.x * Image::dimensions.y; };
	unsigned char* data;
	glm::ivec2 dimensions;
	TextureID ID;
};

