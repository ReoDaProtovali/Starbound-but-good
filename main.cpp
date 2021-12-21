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
	int tempGenCount = 20;
	for (int i = 0; i < tempGenCount; i++) {
		world.genChunk(i % 5, i / 4);
		chunkArr.push_back(world.getChunk(glm::ivec2(i % 5, i / 4)));
	}
	Uint32 updateTicks = 0;
	Uint32 renderTicks = 0;
	glm::vec2 lastPos = glm::vec2(0, 0);
	glm::vec2 currentPos = glm::vec2(0, 0);
	// need to make a separate var to hold the camera pos, so it can be drawn with an offset
	glm::vec2 camPosTest = glm::vec2(200.0f + cos((SDL_GetTicks() / 1000.0f)) * 20, -128.0f + sin((SDL_GetTicks() / 1000.0f) * 1.5) * 5);

	Timestepper ts = Timestepper(4); // sets the game update loop fps, at 8fps
	while (gameActive) {
		ts.processFrameStart();
		while (ts.accumulatorFull()) {
			ts.accumulator = 0.0f;
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
			// game update code should go right here I think (limited to ts.gameUpdateFPS)

			float renderFrametime = (SDL_GetTicks() - updateTicks) / 1000.0; // update frametime in seconds
			updateTicks = SDL_GetTicks();

			float seconds = (SDL_GetTicks() / 1000.0f);

			lastPos = currentPos;
			camPosTest = glm::vec2(200.0f + cos(seconds) * 20.0f, -128.0f + sin(seconds) * 20.0f); // move cam in a circle
			currentPos = camPosTest;
			//printf("%f", ts.alpha);
		}
		ts.calculateAlpha();
		float renderFrametime = (SDL_GetTicks() - renderTicks) / 1000.0; // render frametime in seconds
		renderTicks = SDL_GetTicks();

		// render code goes here I think
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the window

		// interpolates camera pos between game updates by guessing it based on the last position and alpha, test code
		cam.setGlobalPos(utils::lerp(camPosTest, lastPos, ts.alpha));



		for (int i = 0; i < tempGenCount; i++) { // god i hope this works
			renderer.drawChunk(chunkArr[i], gw);
		}

		SDL_GL_SwapWindow(window); // Put the image buffer into the window

		ts.processFrameEnd(gw); // for the timestepper (limited to vsync)
	}

	world.logChunks();
	chunkArr.clear();
	gw.cleanUp();
	SDL_Quit();

	return 0;
}
