#include <SDL.h>
#include <iostream>
#include "utils.hpp"
#include "GameConstants.hpp"
#include "GameWindow.hpp"
#include "GameRenderer.hpp"
#include "Entity.hpp"
//#include "GLTesting.hpp"
#include <cmath>
#include "World.hpp"
#include "Timestepper.hpp"
#include "ResourceLoader.hpp"
#include "GameAssets.hpp"
#include "InputHandler.hpp"
#include "Shader.hpp"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>


int main(int argc, char* argv[])
{
	bool gameActive = true;
	SDL_Event event;

	GameWindow gw = GameWindow("Borstoind");
	SDL_Window* window = gw.window;

	GameRenderer renderer = GameRenderer();
	Camera& cam = renderer.cam;
	//renderer.cam.setTileScale(16); // Sets the display width of a single tile to be 16px
	renderer.cam.pos = glm::vec3(0.0, 0.0, 100.0); // testing having the cam further away

	World world = World();

	std::vector<WorldChunk> chunkArr;
	int tempGenCount = 60;
	for (int i = 0; i < tempGenCount; i++) {
		world.genChunk(i % (int)sqrt(tempGenCount), i / (int)sqrt(tempGenCount));
		chunkArr.push_back(world.getChunk(glm::ivec2(i % (int)sqrt(tempGenCount), i / (int)sqrt(tempGenCount))));
	}


	Timestepper ts = Timestepper(60, gw.getRefreshRate()); // sets the game update loop fps, and you pass in the vsync fps for ease of use
	int printConsoleCounter = 0;
	fpsGauge updateFPSGauge;
	fpsGauge renderFPSGauge;
	std::vector<float> updateFPSVec;
	std::vector<float> renderFPSVec;
	while (gameActive) {
		ts.processFrameStart();
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) { // disables the game loop if you hit the window's x button
				gameActive = false;
			}
			else if (event.type == SDL_KEYDOWN) {
				gw.inpHandler.processKeyDown(event.key.keysym.sym);
			}
			else if (event.type == SDL_KEYUP) {
				gw.inpHandler.processKeyUp(event.key.keysym.sym);
				if (event.key.keysym.sym == SDLK_ESCAPE)
					gameActive = false;
				break;
			}
		}
		printConsoleCounter++;

		while (ts.accumulatorFull()) {
			ts.accumulator -= 1.0f / ts.gameUpdateFPS;
			// game update code should go right here I think (limited to ts.gameUpdateFPS)

			updateFPSGauge.stopStopwatch();
			updateFPSGauge.startStopwatch();
			//if (updateFPSGauge.getSecondsElapsed() != 0) {
				updateFPSVec.push_back(1.0f / updateFPSGauge.getSecondsElapsed());
			//}
			if (updateFPSVec.size() > (size_t)ts.gameUpdateFPS / 4) { // quarter second fps buffer
				updateFPSVec.erase(updateFPSVec.begin());
			}

			if (printConsoleCounter > ts.renderFPS) {
				printConsoleCounter = 0;
				system("CLS");
				printf("Current Update FPS: %f \n", utils::averageVector(updateFPSVec));
				printf("Current Draw FPS: %f \n", utils::averageVector(renderFPSVec));
				//utils::logVector(updateFPSVec);
			}
			ts.processFrameStart();

		}
		ts.calculateAlpha();

		// render code goes here I think
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the window

		renderFPSGauge.stopStopwatch();
		renderFPSGauge.startStopwatch();
		//if (renderFPSGauge.getSecondsElapsed() != 0) {
			renderFPSVec.push_back(1.0f / renderFPSGauge.getSecondsElapsed());
		//}
		if (renderFPSVec.size() > (size_t)ts.renderFPS / 4) { // quarter second fps display buffer
			renderFPSVec.erase(renderFPSVec.begin());
		}

		cam.scale = glm::vec2(0.5, 0.5);

		if (gw.inpHandler.testKey(SDLK_w)) {
			cam.pos.y += 1;
		}
		if (gw.inpHandler.testKey(SDLK_a)) {
			cam.pos.x -= 1;
		}
		if (gw.inpHandler.testKey(SDLK_s)) {
			cam.pos.y -= 1;
		}
		if (gw.inpHandler.testKey(SDLK_d)) {
			cam.pos.x += 1;
		}

		for (int i = 0; i < tempGenCount; i++) { // god i hope this works
			renderer.drawChunk(chunkArr[i], gw);
		}

		SDL_GL_SwapWindow(window); // Put the image buffer into the window
	}

	world.logChunks();
	chunkArr.clear();
	gw.cleanUp();
	SDL_Quit();

	return 0;
}