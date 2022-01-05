#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <SDL.h>
#include <iostream>
#include "utils.hpp"
#include "GameConstants.hpp"
#include "GameWindow.hpp"
#include "GameRenderer.hpp"
#include "Entity.hpp"

#include <cmath>
#include "World.hpp"
#include "Timestepper.hpp"
#include "ResourceLoader.hpp"
#include "GameAssets.hpp"
#include "InputHandler.hpp"
#include "Shader.hpp"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Pixmap.hpp"
#include <semaphore>
#include <thread>
#include <chrono>


int main(int argc, char* argv[])
{
	bool gameActive = true;
	SDL_Event event;


#ifdef _DEBUG
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	flag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(flag);
#endif

	GameWindow gw = GameWindow("Borstoind");
	SDL_Window* window = gw.window;


	SDL_DisplayMode sm;
	SDL_GetDesktopDisplayMode(0, &sm);
	int ww, wh;
	SDL_GetWindowSize(window, &ww, &wh);
	GameRenderer renderer = GameRenderer(sm.w, sm.h, ww, wh);

	Camera& cam = renderer.cam;
	cam.tileScale = 128.0f;
	cam.updateFrame(wh, ww);

	//cam.updateFrame((float)gw.width, (float)gw.height); // actually, this is probably unneeded

	glm::vec2 camVelocity = glm::vec2(0.0f, 0.0f);

	World world = World();

	Timestepper ts = Timestepper(5, gw.getRefreshRate()); // sets the game update loop fps, and you pass in the vsync fps for ease of use
	int printConsoleCounter = 0; // to limit the amount the console updates as to not cause lag
	fpsGauge updateFPSGauge;
	fpsGauge renderFPSGauge;
	std::vector<float> updateFPSVec;
	std::vector<float> renderFPSVec;
	unsigned int frame = 0;
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

					int w = event.window.data1;
					int h = event.window.data2;
					gw.width = w;
					gw.height = h;
					renderer.windowWidth = w;
					renderer.windowHeight = h;
					renderer.rescale();
					glViewport(0, 0, w, h);
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
		frame++;
		while (ts.accumulatorFull()) {
			ts.accumulator -= 1.0f / ts.gameUpdateFPS;
			// game update code should go right here I think (limited to ts.gameUpdateFPS)
			if ((frame < 2600) && (frame > 230)) world.autoGen(renderer.cam);

			updateFPSGauge.stopStopwatch(); // round trip time the update frame took
			updateFPSGauge.startStopwatch();
			updateFPSVec.push_back(1.0f / updateFPSGauge.getSecondsElapsed()); // Used to get the average frametime

			if (updateFPSVec.size() > (size_t)ts.gameUpdateFPS / 4) { // quarter second fps counter buffer
				updateFPSVec.erase(updateFPSVec.begin());
			}

			if (printConsoleCounter > ts.renderFPS) { // means the console updates every second
				printConsoleCounter = 0;
				system("CLS");
				printf("Current Update FPS: %f \n", utils::averageVector(updateFPSVec));
				printf("Current Draw FPS: %f \n", utils::averageVector(renderFPSVec));
				printf("Cam Position: %f, %f \n", cam.pos.x, cam.pos.y);
				printf("Cam Frame: X range-%f, %f   Y range-%f, %f\n", cam.getFrame().x, cam.getFrame().z, cam.getFrame().y, cam.getFrame().w);
				printf("Screen Dimensions: Width-%i Height-%i\n", renderer.screenWidth, renderer.screenHeight);
				printf("Window Dimensions: Width-%i Height-%i\n", renderer.windowWidth, renderer.windowHeight);

			}

			ts.processFrameStart();
		}


		ts.calculateAlpha();

		renderFPSGauge.stopStopwatch();
		renderFPSGauge.startStopwatch();
		renderFPSVec.push_back(1.0f / renderFPSGauge.getSecondsElapsed());

		if (renderFPSVec.size() > (size_t)ts.renderFPS / 4) { // quarter second fps display buffer
			renderFPSVec.erase(renderFPSVec.begin());
		}

		//cam.scale = glm::vec2(0.5, 0.5);

		if (gw.inpHandler.testKey(SDLK_w)) {
			camVelocity.y += 0.1f;
		}
		if (gw.inpHandler.testKey(SDLK_a)) {
			camVelocity.x -= 0.1f;
		}
		if (gw.inpHandler.testKey(SDLK_s)) {
			camVelocity.y -= 0.1f;
		}
		if (gw.inpHandler.testKey(SDLK_d)) {
			camVelocity.x += 0.1f;
		}

		if (gw.inpHandler.testKey(SDLK_q)) {
			cam.tileScale *= 0.992f;
		}
		if (gw.inpHandler.testKey(SDLK_e)) {
			cam.tileScale *= 1.01f;
		}

		if (gw.inpHandler.testKey(SDLK_1)) {
			world.removeChunks();
		}

		camVelocity *= 0.9;
		cam.pos += glm::vec3(camVelocity, 0.0f);
		cam.lookAt(glm::vec3(0.0, 0.0, 0.0));

		renderer.bindScreenFBOAsRenderTarget();
		glClearColor(0.8f, 0.8f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		world.drawWorld(renderer, gw);

		gw.bindAsRenderTarget();
		glDrawBuffer(GL_BACK);
		glDisable(GL_DEPTH_TEST);

		renderer.doLighting();

		SDL_GL_SwapWindow(window); // Put the image buffer into the window
	}

	//world.logChunks();
	gw.cleanUp();
	SDL_Quit();
	world.removeChunks();

	return 0;
}
