#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include "GameConstants.hpp"
#include "Image.hpp"

class ResourceLoader
{
public:
	ResourceLoader() {};
	bool load(const char* p_filepath, TextureID p_assignedID);
	Image getImage(TextureID p_ID); // throws exception if not found
	Image getImage(TextureID p_ID, bool& success); // friendlier, success bool is passed by reference

private:
	std::vector<Image> images;
};

