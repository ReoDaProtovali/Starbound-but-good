#pragma once
#include <iostream>
#include "GameConstants.hpp"
#include "Framework/Graphics/Texture.hpp"

class AtlasBuilder {

	int spriteWidth;
	int spriteHeight;
	int columns = 1;

	GLuint atlasFBO;
	Texture atlasColorTex;

	void initFBO();
};