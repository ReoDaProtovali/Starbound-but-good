#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include "GameConstants.hpp"
#include "Texture.hpp"
#include <map>

class ResourceLoader
{
public:
	ResourceLoader() {};
	bool load(const char* p_filepath, TextureID p_assignedID);
	Texture getTexture(TextureID p_ID); // throws exception if not found
	Texture getTexture(TextureID p_ID, bool& success); // friendlier, success bool is passed by reference

private:
	std::map<TextureID, Texture> textures;
};

