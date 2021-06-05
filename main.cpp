#include <SDL.h>
#include <iostream>
#include <SDL_image.h>
#include "GameWindow.hpp"
#include "Entity.hpp"
#include "Mathutils.hpp"
#include "utils.h"
#include "World.hpp"
#include "Timestepper.h"
#include "ResourceLoader.hpp"
#include "InputHandler.h"

int main(int argc, char* argv[]) {
	bool gameActive = true;
	SDL_Event event;

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) > 0) {
		std::cout << "somethin done goofed and sdl aint runnin chief: " << SDL_GetError() << std::endl;
	}
	if (!IMG_Init(IMG_INIT_PNG)) {
		std::cout << "somethin done goofed and SDL_image aint runnin gamer: " << IMG_GetError() << std::endl;
	}

	GameWindow window = GameWindow("Borstoind", 1280, 720);

	window.res.load("res/tiles/teststone.png"); // assigned id 1
	window.res.load("res/tiles/testdirt.png"); // assigned id 2
	window.res.load("res/tiles/tile1.png"); // assigned id 3
	Uint16 meFound = window.res.load("res/me.png"); // assigned id 4

	if (!meFound) {
		gameActive = false;
	}

	World world = World();
	WorldChunk& chunk = world.getChunk(Vector2i(0, 0));
	WorldChunk& chunk1 = world.getChunk(Vector2i(1, 0));
	chunk.fillRandom();
	chunk1.fillRandom();


	Timestepper ts = Timestepper(SIXTY_TIMES_PER_SECOND); // limits updates to 60fps, multiply by some number to make it a fraction of 60, divide to make it a multiple

	while (gameActive) {
		ts.processFrameStart();

		while (ts.accumulatorFull()) {

			while (SDL_PollEvent(&event)) { 
				if (event.type == SDL_QUIT) { // disables the game loop if you hit the window's x button
					gameActive = false;
				}
				else if (event.type == SDL_KEYDOWN){
					window.inpHandler.processKeyDown(event.key.keysym.sym);
				}
				else if (event.type == SDL_KEYUP) {
					window.inpHandler.processKeyUp(event.key.keysym.sym);
				}
			}
			// game update code should go right here I think
			window.updateCamera();
			ts.accumulator -= ts.timeStep;
		}
		ts.calculateAlpha(); 

		// render code goes here I think
		window.clear();
		window.drawChunk(world.getChunk(Vector2i(0, 0)));
		window.drawChunk(world.getChunk(Vector2i(1, 0)));
		window.display();
		ts.processFrameEnd(window);
	}

	window.cleanUp();
	world.~World();
	SDL_Quit();

	return 0;
}