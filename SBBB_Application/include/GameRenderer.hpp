#pragma once
#include <iostream>
#include <vector>

// Rendering handling
#include "Camera.hpp"
#include <util/ext/glm/glm.hpp>
#include "Lighting.hpp"
#include "Framework/Graphics/FrameBuffer.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include "Framework/Window/GameWindow.hpp"
#include "Framework/Log.hpp"

#include "Chunk.hpp"
#include "ChunkManager.hpp"
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
	GameRenderer();
	GameRenderer(const GameWindow& p_window);
	~GameRenderer();

	uint32_t windowWidth;
	uint32_t windowHeight;
	uint32_t screenWidth;
	uint32_t screenHeight;

	// Temporary until a better texture loader is devised.
	void loadTextures();

	/// Sets the class-defined m_screenFBO as the render target for subsequent gl draw calls.
	void bindScreenFBOAsRenderTarget();

	// temporary
	void setClearColor(glm::vec4 p_col);

	void clearScreen();
	/// Handles what to do when the window changes size, relies on the values of windowWidth and windowHeight being up to date.
	void rescale();

	int drawWorld(ChunkManager& p_world, DrawSurface& p_target);

	/// Simply tells the lighting subclass to draw based on it's current values.
	void drawLighting();

	// TEST ----------------------------------------------------------------------------
	/// The camera is bound directly to the renderer for now.
	// The cameras are pointers, such that they can be easily switched between
	std::shared_ptr<Camera> cam = std::make_shared<Camera>();
	std::shared_ptr<Camera> overviewCam;

	Sprite testReoSprite = Sprite(glm::vec3(-16.0f, 315.0f, 2.0f), Rect(0.f, 0.f, 3.f, 3.f));
	std::shared_ptr<Texture> testReoTexture;
	Sprite cameraFrameSprite = Sprite(glm::vec3(-16.0f, 315.0f, 2.0f), Rect(0, 0, cam->getFrameDimensions().x, cam->getFrameDimensions().y));
	std::shared_ptr<Texture> cameraFrameTexture;

	float testFrame = 0;
	void testDraw();
	void swapCameras();

	// ---------------------------------------------------------------------------------


	Lighting m_lighting;

private:

	GenericShaders& gs = GenericShaders::Get();
	std::weak_ptr<Camera> currentCamera;
	bool cameraToggle = false;

	ResourceLoader res;

	std::shared_ptr<Texture> tileSheetTexture;

	/// Uses packed chunk coordinates and block IDs
	std::shared_ptr<Shader> m_tileShader;
	FrameBuffer m_screenFBO;

	DrawStates m_worldDrawStates;

};

