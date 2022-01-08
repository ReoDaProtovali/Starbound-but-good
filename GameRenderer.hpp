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

/// Serves as a wrapper for all of the rendering subsystems
/// Keeps track of the window dimensions for ease of rendering
/// Stores default spritesheets, and a resource loader that holds various textures.
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

	/// Used for rendering the screen to a separate texture, so the lighting texture can be overlaid.
	GLuint screenFBO;
	/// Texture that contains the screen color before lighting or other overlay effects
	Texture screenColorTex;
	/// The GL buffer ID for the screen's depth buffer.
	GLuint depthBuffer;
	/// Color attatchments being drawn to by the shaders, for multiple shader outputs if required
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };

	/// Loads each of the pre - defined spritesheets, temporary until a more modular system is made
	void loadSpriteSheets();

	/// Creates the Frame Buffer Object, and binds the screenColorTex and depthBuffer.
	void initFBO();

	/// Sets the class-defined screenFBO as the render target for subsequent gl draw calls.
	void bindScreenFBOAsRenderTarget();

	/// Handles what to do when the window changes size, relies on the values of windowWidth and windowHeight being up to date.
	void rescale();

	/// The camera bound directly to the game renderer.
	Camera cam;

	/// The shader program that simply draws an image based on given vertex UVs.
	Shader imageShader;
	
	/**
	* Draws a single WorldChunk to the currently bound buffer.
	* 
	* @param p_chunk - A reference to a WorldChunk
	* @returns A bool that indicated if the chunk was successfully drawn. Currently only returns true.
	*/
	bool drawChunk(WorldChunk& p_chunk);

	/// Unimplemented
	bool drawSprite(glm::vec2 p_worldPos, Texture p_spriteTex);
	
	/// Unimplemented
	bool drawSprite(float p_spriteX, float p_spriteY, Texture p_spriteTex);

	/// Simply tells the lighting subclass to draw based on it's current values.
	void drawLighting();
};

