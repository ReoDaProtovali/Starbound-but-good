#include <SDL.h>
#include <iostream>
#include <SDL_image.h>
#include "renderwindow.hpp"
#include "entity.hpp"

int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) > 0) {
		std::cout << "somethin done goofed and sdl aint runnin chief: " << SDL_GetError() << std::endl;
	}

	if (!IMG_Init(IMG_INIT_PNG)) {
		std::cout << "somethin done goofed and SDL_image aint runnin gamer: " << IMG_GetError() << std::endl;
	}

	RenderWindow window = RenderWindow("Borstoind", 500, 500);
	SDL_Texture* testTex = window.loadTexture("res/testsprites/tile1.png");

	Entity test0(100, 100, testTex);
	Entity test1(164, 100, testTex);
	Entity test2(228, 100, testTex);
	Entity test3(292, 100, testTex);

	bool gameActive = true;
	SDL_Event event;

	while (gameActive) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				gameActive = false;
			}
		}
		window.clear();
		window.render(test0);
		window.render(test1);
		window.render(test2);
		window.render(test3);
		window.display();
	}

	window.cleanUp();
	SDL_Quit();

	return 0;
}