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

	GameWindow gw = GameWindow("Borstoind", 1000, 1000);
	SDL_Window* window = gw.window;

	GameRenderer renderer = GameRenderer();
	renderer.cam.setTileScale(16); // Sets the display width of a single tile to be 16px

	World world = World();
	WorldChunk& chunk = world.getChunk(glm::ivec2(0, 0));
	WorldChunk& chunk1 = world.getChunk(glm::ivec2(1, 0));
	chunk.fillRandom();
	chunk1.fillRandom();

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

		float seconds = SDL_GetTicks() / 1000.0;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the window

		renderer.cam.pos = glm::vec3(
			64 + cos(seconds) * 64,
			- 16 + sin(seconds) * 32,
			1);
		renderer.drawChunk(chunk, gw);
		renderer.drawChunk(chunk1, gw);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // draws the test rectangle

		SDL_GL_SwapWindow(window); // i forgot what this does but I think it just means update the picture

		ts.processFrameEnd(gw); // for the timestepper (limited to vsync)
	}

	gw.cleanUp();
	world.~World();
	SDL_Quit();

	return 0;
}
