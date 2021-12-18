#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include "GameConstants.hpp"
#include "MathUtils.hpp"
#include "Image.hpp"

class ResourceLoader
{
public:
	ResourceLoader() {};
	bool load(const char* p_filepath, TextureID p_assignedID);
	Image getImage(TextureID p_ID);
private:
	std::vector<Image> images;
};

