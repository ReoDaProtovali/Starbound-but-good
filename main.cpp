#include <SDL.h>
#include <iostream>
#include <SDL_image.h>
#include "Renderwindow.hpp"
#include "Entity.hpp"

int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) > 0) {
		std::cout << "somethin done goofed and sdl aint runnin chief: " << SDL_GetError() << std::endl;
	}

	if (!IMG_Init(IMG_INIT_PNG)) {
		std::cout << "somethin done goofed and SDL_image aint runnin gamer: " << IMG_GetError() << std::endl;
	}

	RenderWindow window = RenderWindow("Borstoind", 500, 500);
	SDL_Texture* testTex = window.loadTexture("res/testsprites/tile1.png");

	Entity entities[4] = { 
		Entity(100, 100, testTex),
		Entity(164, 100, testTex),
		Entity(228, 100, testTex),
		Entity(292, 100, testTex) 
	};

	bool gameActive = true;
	SDL_Event event;

	while (gameActive) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				gameActive = false;
			}
		}
		window.clear();
		for (int i = 0; i < 4; i++) {
			window.render(entities[i]);
		}
		window.display();
	}

	window.cleanUp();
	SDL_Quit();

	return 0;
}