#pragma once
#include "Framework/Window/GameWindow.hpp"
#include "GameRenderer.hpp"
#include "WorldRenderer.hpp"
#include "GameStates.hpp"

class ClientWorldState : public GameState {
public:
	ClientWorldState() = delete;
	ClientWorldState(GameWindow& p_window, GameRenderer& p_renderer) :
		window(p_window), renderer(p_renderer) {};

	void init() override {
		// important, opengl NEEDS this
		window.bindToThisThread();
#ifndef DISABLE_DEBUG_STATS
		globals.debug.drawThread = std::this_thread::get_id();
#endif // DISABLE_DEBUG_STATS
	}
	void update() override {
		renderer.bindScreenFBOAsRenderTarget();
		renderer.setClearColor(glm::vec4(0.2f, 0.2f, 0.3f, 0.0f));
		renderer.clearScreen();

		int drawnChunkCount = renderer.drawWorld();
#ifndef DISABLE_DEBUG_STATS
		if (drawnChunkCount != 0) globals.debug.drawnChunkCount = drawnChunkCount;
#endif
		//glEnable(GL_DEPTH_TEST);
		renderer.testDraw();

		window.clear();
		window.bind();
		glDisable(GL_DEPTH_TEST);
		renderer.drawLighting();
		window.displayNewFrame();
	}
	void suspend() override {

	}
	void close() override {

	}

private:
	GameWindow& window;
	GameRenderer& renderer;
	//WorldRenderer worldRenderer;
};