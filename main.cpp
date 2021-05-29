#include <SDL.h>
#include <iostream>
#include <SDL_image.h>
#include "Renderwindow.hpp"
#include "Entity.hpp"
#include "Mathutils.hpp"
#include "utils.h"
#include "World.hpp"

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
	WorldChunk chunk = world.getChunk(Vector2i(0, 0));
	SDL_Texture* testTex = window.loadTexture("res/testsprites/tile1.png");
	SDL_Texture* me = window.loadTexture("res/me.png");

	if (me == NULL) {
		gameActive = false;
	}

	const float timeStep = 1.0f / 60;
	float accumulator = 0.0f;
	float currentTime = utils::hireTimeInSeconds();

	while (gameActive) {
		int startTicks = SDL_GetTicks();
		float newTime = utils::hireTimeInSeconds();
		float frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;

		while (accumulator >= timeStep) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					gameActive = false;
				}
			}
			accumulator -= timeStep;
		}
		const float alpha = accumulator / timeStep;
		window.clear();

		window.display();

		int frameTicks = SDL_GetTicks() - startTicks;
		if (frameTicks < 1000.0f / window.getRefreshRate()) {
			SDL_Delay(1000.0f / window.getRefreshRate() - frameTicks);
		}
	}

	window.cleanUp();
	world.~World();
	SDL_Quit();

	return 0;
}