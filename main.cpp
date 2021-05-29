#include <SDL.h>
#include <iostream>
#include <SDL_image.h>
#include "Renderwindow.hpp"
#include "Entity.hpp"
#include "Mathutils.hpp"
#include "utils.h"
#include "World.hpp"
#include "Timestepper.h"

int main(int argc, char* argv[]) {
	bool gameActive = true;
	SDL_Event event;

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) > 0) {
		std::cout << "somethin done goofed and sdl aint runnin chief: " << SDL_GetError() << std::endl;
	}
	if (!IMG_Init(IMG_INIT_PNG)) {
		std::cout << "somethin done goofed and SDL_image aint runnin gamer: " << IMG_GetError() << std::endl;
	}

	RenderWindow window = RenderWindow("Borstoind", 1280, 720);

	World world = World();
	WorldChunk& chunk = world.getChunk(Vector2i(0, 0));
	chunk.fillRandom();

	SDL_Texture* testTex = window.loadTexture("res/testsprites/tile1.png");
	SDL_Texture* me = window.loadTexture("res/me.png");

	if (me == NULL) {
		gameActive = false;
	}

	Timestepper ts = Timestepper(SIXTY_TIMES_PER_SECOND); // limits updates to 60fps

	while (gameActive) {
		ts.processFrameStart();

		while (ts.isAccumulatorDrained()) {
			// update code should go right here I think

			while (SDL_PollEvent(&event)) { // disables the game loop if you hit the x button
				if (event.type == SDL_QUIT) {
					gameActive = false;
				}
			}
			ts.accumulator -= ts.timeStep;
		}
		ts.calculateAlpha(); 
		std::cout << ts.alpha << std::endl;
		window.clear();
		// render code goes here I think

		window.display();
		ts.processFrameEnd(window);
	}

	window.cleanUp();
	world.~World();
	SDL_Quit();

	return 0;
}