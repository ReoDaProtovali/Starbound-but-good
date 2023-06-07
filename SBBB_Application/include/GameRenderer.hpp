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

#include "GUIButton.hpp"

#include "util/SharedList.hpp"
#include "TestEntity.hpp"
#include "box2d.h"
#include "DefaultFonts.hpp"

class GameWindow;

/// Serves as a wrapper for all of the rendering subsystems
/// Keeps track of the window dimensions for ease of rendering
/// Stores default spritesheets, and a resource loader that holds various textures.
class GameRenderer
{
public:
	GameRenderer() = delete;
	GameRenderer(GameWindow& p_window);

	GameWindow& window;

	// temporary
	void setClearColor(glm::vec4 p_col);

	void clearScreen();
	/// Handles what to do when the window changes size, relies on the values of windowWidth and windowHeight being up to date.
	void setViewport(uint16_t p_w, uint16_t p_h);
	void setCam(Camera* p_cam);

	FrameBuffer screenFBO;
	Camera* cam = nullptr;

private:
	Camera defaultCam; // scuffed
};

