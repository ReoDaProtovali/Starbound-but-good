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
#include "GameWindow.hpp"


class GameRenderer
{
public:
	GameRenderer(glm::vec3 p_camPos);
	//GameRenderer(spriteShader);

	ResourceLoader res;
	SpriteSheet tileSheet;
	SpriteSheet objectSheet;
	SpriteSheet entitySheet;

	void loadSpriteSheets();
	unsigned int bufferImage(Image& p_image);

	Camera cam;

	Shader imageShader;

	bool drawChunk(WorldChunk& p_chunk, GameWindow& p_window);
	//GLuint draw

};

