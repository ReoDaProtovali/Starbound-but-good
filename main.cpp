#include <SDL.h>
#include <iostream>
#include "utils.hpp"
#include "GameConstants.hpp"
#include "GameWindow.hpp"
#include "GameRenderer.hpp"
#include "Entity.hpp"
//#include "GLTesting.hpp"
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
		world.genChunk(i%5, i/5);
		chunkArr.push_back(world.getChunk(glm::ivec2(i%5, i/5)));
	}

	Timestepper ts = Timestepper(SIXTY_TIMES_PER_SECOND); // limits updates to 60fps, multiply by some number to make it a fraction of 60, divide to make it a multiple

	while (gameActive) {
		ts.processFrameStart();

		while (ts.accumulatorFull()) {
			glClear(GL_COLOR_BUFFER_BIT);

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
			// game update code should go right here I think (limited to 60fps)

			ts.accumulator -= ts.timeStep;
		}
		ts.calculateAlpha();

		// render code goes here I think

		float seconds = SDL_GetTicks() / 1000.0f;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the window

		//cam.scale = glm::vec2((sin(seconds) * 0.5 + 1) * 0.06f, (cos(seconds) * 0.5 + 1) * 0.06f);
		cam.setGlobalPos(200.0f + cos(seconds) * 20, -128.0f + sin(seconds * 1.5) * 5);
		//renderer.cam.pos = glm::vec3(
		//	32.0 + cos(seconds / 2.0) * 20.0,
		//	sin(seconds / 2.0) * 20.0,
		//	1.0f);

		//renderer.cam.lookForwards();
		//renderer.cam.enableManualView();
		//renderer.cam.enablePerspective();
		//renderer.cam.lookAt(glm::vec3(32.0f, 0.0f, 0.0f));


		for (int i = 0; i < tempGenCount; i++) { // god i hope this works
			renderer.drawChunk(chunkArr[i], gw);
		}

		SDL_GL_SwapWindow(window); // Put the image buffer into the window

		ts.processFrameEnd(gw); // for the timestepper (limited to vsync)
	}

	gw.cleanUp();
	//world.~World();
	SDL_Quit();

	return 0;
}
