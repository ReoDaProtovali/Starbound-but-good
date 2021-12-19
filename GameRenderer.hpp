#pragma once
#include <iostream>
#include <vector>

// Rendering handling
#include "Shader.hpp" // includes glew
#include "Camera.hpp"
#include "glm/glm.hpp"

// Image handling
#include "ResourceLoader.hpp" 
#include "SpriteSheet.hpp"

// Game data handling
#include "GameConstants.hpp"
#include "Chunk.hpp"
#include "World.hpp"


class GameRenderer
{
	GameRenderer();
	ResourceLoader res;
	SpriteSheet tileSheet;
	SpriteSheet objectSheet;
	SpriteSheet entitySheet;

	Camera worldCam;
	
	Shader spriteShader;
	Shader imageShader;

	GLuint getChunkVAO(WorldChunk& p_chunk);
	//GLuint draw

};

