#include <SDL.h>
#include <iostream>
#include <SDL_image.h>
#include "Renderwindow.hpp"
#include "Entity.hpp"
#include "Mathutils.hpp"

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) > 0) {
		std::cout << "somethin done goofed and sdl aint runnin chief: " << SDL_GetError() << std::endl;
	}

	if (!IMG_Init(IMG_INIT_PNG)) {
		std::cout << "somethin done goofed and SDL_image aint runnin gamer: " << IMG_GetError() << std::endl;
	}

	Vector2f testVector(.70710678f, .70710678f);

	std::cout << testVector.length() << std::endl;

	std::cout << sizeof testVector << std::endl;

	RenderWindow window = RenderWindow("Borstoind", 1280, 720);
	SDL_Texture* testTex = window.loadTexture("res/testsprites/tile1.png");

	std::vector<Entity> entities = {
		Entity(Vector2f(100.0f, 100.0f), testTex, 8, 16),
		Entity(Vector2f(164.0f, 100.0f), testTex),
		Entity(Vector2f(228.0f, 100.0f), testTex),
		Entity(Vector2f(292.0f, 100.0f), testTex)
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
		for (Entity& e : entities) {
			window.render(e);
		}
		window.display();
	}

	window.cleanUp();
	SDL_Quit();

	return 0;
}