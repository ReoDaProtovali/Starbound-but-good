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

#include <semaphore>
#include <thread>

int main(int argc, char* argv[])
{
	bool gameActive = true;
	SDL_Event event;

	GameWindow gw = GameWindow("Borstoind");
	SDL_Window* window = gw.window;

	GameRenderer renderer = GameRenderer(glm::vec3(0.0f, 0.0f, 1.0f));
	Camera& cam = renderer.cam;
	glm::vec2 camVelocity = glm::vec2(0.0f, 0.0f);
	//renderer.cam.setTileScale(16); // Sets the display width of a single tile to be 16px
	renderer.cam.pos = glm::vec3(0.0, 10.0 * CHUNKSIZE, 1.0); // testing having the cam further away
	renderer.cam.tileScale = 128.0;
	cam.updateFrame(gw.width, gw.height);


	World world = World();

	std::vector<WorldChunk> chunkArr;

	Timestepper ts = Timestepper(30, gw.getRefreshRate()); // sets the game update loop fps, and you pass in the vsync fps for ease of use
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
			else if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					SDL_Log("Window %d resized to %dx%d",
						event.window.windowID, event.window.data1,
						event.window.data2);
					gw.width = event.window.data1;
					gw.height = event.window.data2;
					SDL_SetWindowSize(window, gw.width, gw.height);
				}
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
				printf("Cam Position: %f, %f \n", cam.pos.x, cam.pos.y);
			}
			ts.processFrameStart();

		}
		world.autoGen(renderer.cam);

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

		//cam.scale = glm::vec2(0.5, 0.5);

		if (gw.inpHandler.testKey(SDLK_w)) {
			camVelocity.y += 0.1;
		}
		if (gw.inpHandler.testKey(SDLK_a)) {
			camVelocity.x -= 0.1;
		}
		if (gw.inpHandler.testKey(SDLK_s)) {
			camVelocity.y -= 0.1;
		}
		if (gw.inpHandler.testKey(SDLK_d)) {
			camVelocity.x += 0.1;
		}

		if (gw.inpHandler.testKey(SDLK_q)) {
			cam.tileScale *= 0.992f;
		}
		if (gw.inpHandler.testKey(SDLK_e)) {
			cam.tileScale *= 1.01f;
		}
		camVelocity *= 0.9;
		cam.pos += glm::vec3(camVelocity, 0.0f);

		int w, h;
		SDL_GetWindowSize(gw.window, &w, &h);
		cam.updateFrame(w, h);
		cam.lookAt(glm::vec3(0.0, 0.0, 0.0));
		std::map<wc::ivec2, WorldChunk>::iterator it = world.chunkMap.begin();
		while (it != world.chunkMap.end()) {
			//printf("World Pos: %i, %i \n", it->second.worldPos.x, it->second.worldPos.y);
			//printf("Cam Frame Pos1: %f, %f \n", cam.frame.x, cam.frame.y);
			//printf("Cam Frame Pos2: %f, %f \n", cam.frame.z, cam.frame.w);

			glm::vec2 chunkGlobalPos = it->second.worldPos * CHUNKSIZE;
			if (
				//(chunkGlobalPos.x > cam.frame.x) &&
				(chunkGlobalPos.x > cam.frame.x - 1 * CHUNKSIZE) &&
				(chunkGlobalPos.x < cam.frame.z + 1 * CHUNKSIZE) &&
				(chunkGlobalPos.y > cam.frame.y - 1 * CHUNKSIZE) &&
				(chunkGlobalPos.y < cam.frame.w + 1 * CHUNKSIZE)
				) {
				renderer.drawChunk(it->second, gw);
			}
			++it;
		};

		SDL_GL_SwapWindow(window); // Put the image buffer into the window
	}

	world.logChunks();
	chunkArr.clear();
	gw.cleanUp();
	SDL_Quit();

	return 0;
}