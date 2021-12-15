#include <SDL.h>
#include <iostream>
#include <SDL_image.h>
#include "GameWindow.hpp"
#include "Entity.hpp"
#include "Mathutils.hpp"
#include "utils.h"
#include "GLUtils.h"
#include "World.hpp"
#include "Timestepper.h"
#include "ResourceLoader.hpp"
#include "InputHandler.h"

typedef enum gameTexture {
	STONE_TEXTURE,
	DIRT_TEXTURE,
	TILE_TEXTURE,
	REOHANG_TEXTURE
} gameTexture;



int main(int argc, char* argv[])
{
	bool gameActive = true;
	SDL_Event event;

	GameWindow gw = GameWindow("Borstoind", 1280, 720);
	SDL_Window* window = gw.window;

	gw.res.load("res/tiles/teststone.png"); // assigned id 1
    gw.res.load("res/tiles/testdirt.png"); // assigned id 2
    gw.res.load("res/tiles/tile1.png"); // assigned id 3
	gw.res.load("res/reohangsmall.png"); // assigned id 3
	Uint16 meFound = gw.res.load("res/me.png"); // assigned id 4

	if (!meFound) {
		gameActive = false;
	}

	SDL_GLContext& context = gw.glContext;

	GLuint program = compileShaders();

	GLuint vao; 
	handleVertexAttrBuffers(vao, gw.width, gw.height); // hiding a lot of code behind here


	// ------------ test texture code ------------------------------------------------
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // it's in some kind of buffer and that is good enough

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* data = gw.res.getTex(DIRT_TEXTURE);
	Vector2i d = gw.res.getDimensions(DIRT_TEXTURE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, d.x, d.y, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // actually put the image data into the texture buffer
	// WARNING:: very picky about if an image in in RGB format or RBGA format. Try to keep them all RGBA, only at RGB because i am dumb and exported the tile pngs as rgb
	// ----------------------------------------------------------------------------


	World world = World();
	WorldChunk& chunk = world.getChunk(Vector2i(0, 0));
	WorldChunk& chunk1 = world.getChunk(Vector2i(1, 0));
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
						return 0;
					break;
				}
			}
			// game update code should go right here I think (limited to 60fps)

			ts.accumulator -= ts.timeStep;
		}
		ts.calculateAlpha();

		// render code goes here I think
		glClear(GL_COLOR_BUFFER_BIT); // clears the window


		glBindVertexArray(vao); // sets the vertex array object containing the test rectangle to active
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // draws the test rectangle

		SDL_GL_SwapWindow(window); // i forgot what this does but I think it just means update the picture

		ts.processFrameEnd(gw); // for the timestepper (limited to vsync)
	}

	gw.cleanUp();
	world.~World();
	SDL_Quit();

	return 0;
}