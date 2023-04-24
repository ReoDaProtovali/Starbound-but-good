#pragma once
#include <iostream>
#include <vector>

// Rendering handling
#include "Camera.hpp"
#include <util/ext/glm/glm.hpp>
#include "Lighting.hpp"
#include "WorldRenderer.hpp"
#include "Framework/Graphics/FrameBuffer.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include "Framework/Window/GameWindow.hpp"
#include "Framework/Log.hpp"

#include "Chunk.hpp"
#include "ChunkManager.hpp"
// Image handling
#include "ResourceManager.hpp" 
#include "SpriteSheet.hpp"

#include "Framework/Graphics/Sprite.hpp"

#include "Framework/Graphics/Text.hpp"

#include "util/SharedList.hpp"
#include "TestEntity.hpp"
#include "box2d.h"

class Lighting;
class GameWindow;

/// Serves as a wrapper for all of the rendering subsystems
/// Keeps track of the window dimensions for ease of rendering
/// Stores default spritesheets, and a resource loader that holds various textures.
class GameRenderer
{
public:
	GameRenderer() = delete;
	GameRenderer(const GameWindow& p_window);
	~GameRenderer();

	uint32_t windowWidth;
	uint32_t windowHeight;
	uint32_t screenWidth;
	uint32_t screenHeight;

	ResourceManager& res = ResourceManager::Get();

	// Temporary until a better texture loader is devised.
	void loadTextures();

	/// Sets the class-defined m_screenFBO as the render target for subsequent gl draw calls.
	void bindScreenFBOAsRenderTarget();

	// temporary
	void setClearColor(glm::vec4 p_col);

	void clearScreen();
	/// Handles what to do when the window changes size, relies on the values of windowWidth and windowHeight being up to date.
	void setViewport(uint16_t p_w, uint16_t p_h);

	int drawWorld();

	/// Simply tells the lighting subclass to draw based on it's current values.
	void drawLighting();


	// TEST ----------------------------------------------------------------------------
	/// The camera is bound directly to the renderer for now.
	// The cameras are pointers, such that they can be easily switched between
	std::shared_ptr<Camera> cam = std::make_shared<Camera>();
	std::shared_ptr<Camera> overviewCam;
	Camera* playerCam = nullptr;

	Sprite testReoSprite = Sprite(glm::vec3(-16.0f, 109.0f, 2.0f), Rect(0.f, 0.f, 3.f, 3.f));
	Texture* testReoTexture = nullptr;

	
	float testFrame = 0;
	void testDraw();
	void swapCameras();

	SharedList<EntityWrapper>& entities = SharedList<EntityWrapper>::Get();

	// ---------------------------------------------------------------------------------

	Lighting m_lighting;
	FrameBuffer m_screenFBO;

private:

	Observer<MouseEvent> m_mouseObserver;

	WorldRenderer worldRenderer;
	GenericShaders& gs = GenericShaders::Get();

	TextContext ftctx; // freetype library context
	Font videotype = { "res/fonts/videotype.ttf", ftctx };

	std::weak_ptr<Camera> currentCamera;
	bool cameraToggle = false;
};

