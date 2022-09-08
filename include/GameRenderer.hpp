#pragma once
#include <iostream>
#include <vector>

// Rendering handling
#include "Camera.hpp"
#include <util/ext/glm/glm.hpp>
#include "Lighting.hpp"
#include "Framework/Graphics/FrameBuffer.hpp"

#include "Chunk.hpp"
// Image handling
#include "ResourceLoader.hpp" 
#include "SpriteSheet.hpp"


class Lighting;
class GameWindow;

/// Serves as a wrapper for all of the rendering subsystems
/// Keeps track of the window dimensions for ease of rendering
/// Stores default spritesheets, and a resource loader that holds various textures.
class GameRenderer
{
public:
	GameRenderer(GameWindow& p_window);

	unsigned int windowWidth;
	unsigned int windowHeight;
	unsigned int screenWidth;
	unsigned int screenHeight;

	ResourceLoader res;
	Lighting lighting;
	//SpriteSheet tileSheet;
	//SpriteSheet objectSheet;
	//SpriteSheet entitySheet;
	Texture tileSheetTexture;

	Mesh<GLfloat> genericSpriteMesh;
	Texture cameraFrameTexture;

	/// Used for rendering the screen to a separate texture, so the lighting texture can be overlaid.
	FrameBuffer screenFBO;

	/// Sets the class-defined screenFBO as the render target for subsequent gl draw calls.
	void bindScreenFBOAsRenderTarget();

	/// Handles what to do when the window changes size, relies on the values of windowWidth and windowHeight being up to date.
	void rescale();

	/// The camera bound directly to the game renderer.
	Camera cam;

	/// The shader program that simply draws an image based on given vertex UVs.
	Shader imageShader;
	/// Uses packed chunk coordinates and block IDs
	Shader tileShader;
	
	/**
	* Draws a single WorldChunk to the currently bound buffer.
	* 
	* @param p_chunk - A reference to a WorldChunk
	* @returns A bool that indicated if the chunk was successfully drawn. Currently only returns true.
	*/
	bool drawChunk(WorldChunk& p_chunk);

	void drawSprite(glm::vec3 p_worldPos, glm::vec2 p_dimensions, Texture p_spriteTex);

	/// Simply tells the lighting subclass to draw based on it's current values.
	void drawLighting();
};

