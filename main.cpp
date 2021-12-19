#include <SDL.h>
#include <iostream>
#include "utils.hpp"
#include "GameConstants.hpp"
#include "GameWindow.hpp"
#include "Entity.hpp"
#include "Mathutils.hpp"
#include "GLTesting.hpp"
#include "World.hpp"
#include "Timestepper.hpp"
#include "ResourceLoader.hpp"
#include "GameAssets.hpp"
#include "InputHandler.hpp"
#include "Shader.hpp"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

std::vector<Vertex> vertices1 = {
	// positions             / / texture coords
	Vertex(glm::vec3(0.5, 0.5, 0.0), glm::vec2(1.0, 1.0)),   // top right
	Vertex(glm::vec3(0.5, -0.5, 0.0), glm::vec2(1.0, 0.0)), // bottom right
	Vertex(glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0)), // bottom left
	Vertex(glm::vec3(-0.5, 0.5, 0.0), glm::vec2(0.0, 1.0)) // top left
};
std::vector<Vertex> vertices2 = {
	// positions             / / texture coords
	Vertex(glm::vec3(0.5, 0.5, 0.0), glm::vec2(1.0, 1.0)),   // top right
	Vertex(glm::vec3(0.5, -0.5, 0.0), glm::vec2(1.0, 0.0)), // bottom right
	Vertex(glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0)), // bottom left
	Vertex(glm::vec3(-0.5, 0.5, 0.0), glm::vec2(0.0, 1.0)) // top left
};

std::vector<GLuint> indices1 = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3  // second triangle
};

int main(int argc, char* argv[])
{
	bool gameActive = true;
	SDL_Event event;

	GameWindow gw = GameWindow("Borstoind", 1000, 1000);
	SDL_Window* window = gw.window;

	ResourceLoader res = ResourceLoader();

	bool success = rl::loadRes(res);
	if (!success) {
		std::cout << "One or more images failed to load!" << std::endl;
	}

	bool meFound = res.load("res/me.png", TextureID::ME_TEXTURE);

	if (!meFound) {
		gameActive = false;
	}


	World world = World();
	WorldChunk& chunk = world.getChunk(glm::ivec2(0, 0));
	WorldChunk& chunk1 = world.getChunk(glm::ivec2(1, 0));
	chunk.fillRandom();
	chunk1.fillRandom();

	SpriteSheet tileSheet = SpriteSheet(res.getImage(TextureID::TILESHEET_TEXTURE), glm::ivec2(8, 8), 1);
	Shader imageShader = Shader("./Shaders/ImageVS.glsl", "./Shaders/ImageFS.glsl");
	GLuint vao; 
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	bufferImage(TextureID::TILESHEET_TEXTURE, res);
	GLuint chunk1VBO = chunk1.generateVBO(tileSheet);

	// vertices1 and indices1 define two triangles, forming the square
	//handleVertexAttrBuffers(vao, vertices1); // hiding a lot of code behind here


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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the window

		glBindVertexArray(vao); // sets the vertex array object containing the chunk data

		glm::mat4 modelTransform = glm::mat4(1.0f);
		modelTransform = glm::translate(modelTransform, glm::vec3(cos(SDL_GetTicks() / 2000.0), sin(SDL_GetTicks() / 2000.0), 0.0f));
		modelTransform = glm::scale(modelTransform, glm::vec3(0.03, 0.03, 0.03));

		imageShader.setMat4Uniform("model", modelTransform);
		glDrawArrays(GL_TRIANGLES, 0, chunk1.verts.size());
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // draws the test rectangle

		SDL_GL_SwapWindow(window); // i forgot what this does but I think it just means update the picture

		ts.processFrameEnd(gw); // for the timestepper (limited to vsync)
	}

	gw.cleanUp();
	world.~World();
	SDL_Quit();

	return 0;
}
