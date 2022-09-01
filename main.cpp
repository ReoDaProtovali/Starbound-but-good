#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <SDL.h>
#include <iostream>
#include "GameWindow.hpp"
#include "GameRenderer.hpp"

#include <cmath>
#include "World.hpp"
#include "Timestepper.hpp"
#include "InputHandler.hpp"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#define GAME_UPDATE_SPEED 60
#define FRAMES_BETWEEN_STAT_UPDATES 120
#define DISABLE_RUNTIME_CONSOLE true

/// Used for console stats
int printConsoleCounter = 0; // to limit the amount the console updates as to not cause lag
fpsGauge updateFPSGauge;
fpsGauge renderFPSGauge;
void sendConsoleStats(Timestepper& ts, GameRenderer& renderer);


// Declarations in advance for main functions
void processTestInputs(InputHandler& inp, World& world, Camera& cam, glm::vec2& camVelocity);
void gameRender(GameRenderer& renderer, GameWindow& gw, World& world);
void gameUpdate(World& world, Camera& cam, int updateFrame);

// Create the game window
GameWindow gw = GameWindow("Borstoind");


int main(int argc, char* argv[])
{
	bool gameActive = true;
	SDL_Event event;

	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // memory leak detection
	flag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(flag);

	GameRenderer renderer = GameRenderer(gw);

	Camera& cam = renderer.cam; // aliasing the renderer camera for convenience
	cam.tileScale = 128.0f; // Immediately changes the camera's scale to 128 tiles across

	glm::vec2 camVelocity = glm::vec2(0.0f, 0.0f); // Temporary stand-in

#ifdef LOADLOGGING_ENABLED
	std::cout << "Creating world container..." << std::endl;
#endif
	World world = World();

	Timestepper ts = Timestepper(GAME_UPDATE_SPEED, gw.getRefreshRate()); // sets the game update loop fps and vsync fps

#ifdef LOADLOGGING_ENABLED
	std::cout << "Main loop running." << std::endl;
#endif
	unsigned int renderFrame = 0;
	unsigned int updateFrame = 0;
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
		renderFrame++;
		while (ts.accumulatorFull()) {
			ts.accumulator -= 1.0f / ts.gameUpdateFPS;
			updateFrame++;
			// Code to execute every update frame
			gameUpdate(world, renderer.cam, updateFrame);

			updateFPSGauge.update(ts.gameUpdateFPS / 4);

			if ((printConsoleCounter > FRAMES_BETWEEN_STAT_UPDATES) && !DISABLE_RUNTIME_CONSOLE) { // means the console updates every second
				printConsoleCounter = 0;
				sendConsoleStats(ts, renderer);
			}

			ts.processFrameStart();
		}


		ts.calculateAlpha();

		renderFPSGauge.update(ts.renderFPS / 4);

		processTestInputs(gw.inpHandler, world, cam, camVelocity);
		camVelocity *= 0.9;
		cam.pos += glm::vec3(camVelocity, 0.0f);

		gameRender(renderer, gw, world);

		gw.displayNewFrame();
	}

	//world.logChunks();
	gw.cleanUp();
	SDL_Quit();
	world.removeChunks();

	return 0;
}


void gameRender(GameRenderer& renderer, GameWindow& gw, World& world) {

	renderer.bindScreenFBOAsRenderTarget();
	glClearColor(0.8f, 0.8f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	world.drawWorld(renderer, gw);

	gw.bindAsRenderTarget();
	glDrawBuffer(GL_BACK);
	glDisable(GL_DEPTH_TEST);

	renderer.drawLighting();
}

void gameUpdate(World& world, Camera& cam, int updateFrame) {

	world.autoGen(cam);
	if (updateFrame % 2 == 0) { // Generate a chunk every fourth update frame
		world.genFromQueue();
	}
}

void processTestInputs(InputHandler& inp, World& world, Camera& cam, glm::vec2& camVelocity) {
	if (inp.testKey(SDLK_w)) {
		camVelocity.y += 0.1f;
	}
	if (inp.testKey(SDLK_a)) {
		camVelocity.x -= 0.1f;
	}
	if (inp.testKey(SDLK_s)) {
		camVelocity.y -= 0.1f;
	}
	if (inp.testKey(SDLK_d)) {
		camVelocity.x += 0.1f;
	}

	if (inp.testKey(SDLK_q)) {
		cam.tileScale *= 0.992f;
	}
	if (inp.testKey(SDLK_e)) {
		cam.tileScale *= 1.01f;
	}

	if (inp.testKey(SDLK_1)) {
		world.removeChunks();
	}
};


void sendConsoleStats(Timestepper& ts, GameRenderer& renderer) {
	system("CLS");
	printf("Current Update FPS - %.2f \n", 1.0f / utils::averageVector(updateFPSGauge.frametimeBuffer));
	printf("Current Draw FPS - %.2f \n", 1.0f / utils::averageVector(renderFPSGauge.frametimeBuffer));
	printf("Cam Position - %.2f, %.2f \n", renderer.cam.pos.x, renderer.cam.pos.y);
	printf("Cam Frame - X range: %.2f, %.2f   Y range: %.2f, %.2f\n", renderer.cam.getFrame().x, renderer.cam.getFrame().z, renderer.cam.getFrame().y, renderer.cam.getFrame().w);
	printf("Screen Dimensions - Width: %i Height: %i\n", renderer.screenWidth, renderer.screenHeight);
	printf("Window Dimensions - Width: %i Height: %i\n", renderer.windowWidth, renderer.windowHeight);
}
