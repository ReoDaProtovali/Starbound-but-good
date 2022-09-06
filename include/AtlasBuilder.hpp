#pragma once
#include "glm/glm.hpp"
#include <iostream>
#include "GameConstants.hpp"
#include "Texture.hpp"

class AtlasBuilder {

	int spriteWidth;
	int spriteHeight;
	int columns = 1;

	GLuint atlasFBO;
	Texture atlasColorTex;

	void initFBO();
};