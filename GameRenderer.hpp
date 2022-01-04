#pragma once
#include <iostream>
#include <vector>

// Rendering handling
#include "Shader.hpp" // includes glew
#include "Camera.hpp"
#include "glm/glm.hpp"
#include "Lighting.hpp"

// Image handling
#include "ResourceLoader.hpp" 
#include "SpriteSheet.hpp"
#include "Texture.hpp"

// Game data handling
#include "GameConstants.hpp"
#include "Chunk.hpp"

class Lighting;
class GameWindow;
class GameRenderer
{
public:
	GameRenderer(
		unsigned int p_screenWidth,
		unsigned int p_screenHeight,
		unsigned int p_windowWidth,
		unsigned int p_windowHeight);

	unsigned int windowWidth;
	unsigned int windowHeight;
	unsigned int screenWidth;
	unsigned int screenHeight;

	ResourceLoader res;
	Lighting lighting;
	SpriteSheet tileSheet;
	SpriteSheet objectSheet;
	SpriteSheet entitySheet;
	Texture tileSheetTexture;

	GLuint screenFBO;
	Texture screenColorTex;
	GLuint depthBuffer; // shouldn't need to be used, unless for testing purposes

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 }; // only one color attachment (right now)


	void loadSpriteSheets();
	void initFBO();

	void bindScreenFBOAsRenderTarget();

	void rescale();

	Camera cam;

	Shader imageShader;

	bool drawChunk(WorldChunk& p_chunk);
	void doLighting();
};

