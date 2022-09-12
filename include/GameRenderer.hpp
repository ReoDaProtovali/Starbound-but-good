#pragma once
#include <iostream>
#include <vector>

// Rendering handling
#include "Camera.hpp"
#include <util/ext/glm/glm.hpp>
#include "Lighting.hpp"
#include "Framework/Graphics/FrameBuffer.hpp"
#include "Framework/Graphics/GenericShaders.hpp"

#include "Chunk.hpp"
// Image handling
#include "ResourceLoader.hpp" 
#include "SpriteSheet.hpp"

#include "Framework/Graphics/Sprite.hpp"


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

	// Temporary until a better texture loader is devised.
	void loadTextures();

	/// Sets the class-defined screenFBO as the render target for subsequent gl draw calls.
	void bindScreenFBOAsRenderTarget();

	// temporary
	void setClearColor(glm::vec4 p_col);
	/// Handles what to do when the window changes size, relies on the values of windowWidth and windowHeight being up to date.
	void rescale();
	
	/**
	* Draws a single WorldChunk to the currently bound buffer.
	* 
	* @param p_chunk - A reference to a WorldChunk
	* @returns A bool that indicated if the chunk was successfully drawn. Currently only returns true.
	*/

	// Will be moved ------------------------------------------------------------
	bool drawChunk(WorldChunk& p_chunk);
	// --------------------------------------------------------------------------

	/// Simply tells the lighting subclass to draw based on it's current values.
	void drawLighting();

	// TEST ----------------------------------------------------------------------------
	Sprite testReoSprite;
	Texture testReoTexture;
	float testFrame = 0;
	void testDraw();


	// ---------------------------------------------------------------------------------

	/// The camera is bound directly to the renderer for now.
	Camera cam;

private:
	/// Used for rendering the screen to a separate texture, so the lighting texture can be overlaid.
	FrameBuffer screenFBO;
	ResourceLoader res;
	// Do not know a better way to group these.
	GenericShaders gs;
	Lighting lighting;
	Texture tileSheetTexture;

	Mesh<GLfloat> genericSpriteMesh;
	Texture cameraFrameTexture;



	/// Uses packed chunk coordinates and block IDs
	Shader tileShader;
	
};

