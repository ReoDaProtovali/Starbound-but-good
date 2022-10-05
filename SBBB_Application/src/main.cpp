#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <SDL.h>
#include "Framework/Window/GameWindow.hpp"
#include "GameRenderer.hpp"
#include "GameConstants.hpp"

#include <cmath>
#include "ChunkManager.hpp"
#include "Timestepper.hpp"
#include "Framework/Input/InputHandler.hpp"


/// Used for console stats
int printConsoleCounter = 0; // to limit the amount the console updates as to not cause lag
int lastChunkDrawnCount = 0;
fpsGauge updateFPSGauge;
fpsGauge renderFPSGauge;
void sendConsoleStats(Timestepper& ts, GameRenderer& renderer, ChunkManager& world);


// Declarations in advance for main functions
void initGL();
void processTestInputs(InputHandler& inp, ChunkManager& world, Camera& cam, glm::vec2& camVelocity, GameRenderer& renderer);
void gameRender(GameRenderer& renderer, GameWindow& gw, ChunkManager& world);
void gameUpdate(ChunkManager& world, Camera& cam, int updateFrame);




int main(int argc, char* argv[])
{
	try {
		bool gameActive = true;
		SDL_Event event;

		LOG("Debug mode active!");

#ifdef SBBB_DEBUG
		int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // memory leak detection
		flag |= _CRTDBG_LEAK_CHECK_DF;
		_CrtSetDbgFlag(flag);
#endif

		// Create the game window
		GameWindow gw = GameWindow("Borstoind");
		initGL();

		GameRenderer renderer = GameRenderer(gw);

		glm::vec2 camVelocity = glm::vec2(0.0f, 0.0f); // Temporary stand-in

		LOAD_LOG("Creating world container...");

		ChunkManager world = ChunkManager();
		//world.genFixed(40, 24);

		Timestepper ts = Timestepper(GAME_UPDATE_SPEED); // sets the game update loop fps
		gw.setVSync(false);

		LOAD_LOG("Main loop running.");

		uint32_t renderFrame = 0;
		uint32_t updateFrame = 0;

		///////////////////////////////////////////////////////
		while (gameActive) {
			ts.processFrameStart();
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) { // disables the game loop if you hit the window's x button
					gameActive = false;
				}
				else if (event.type == SDL_KEYDOWN) {
					gw.inpHandler.processKeyDown(event.key.keysym.sym);
				}
				else if (event.type == SDL_WINDOWEVENT) {
					if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
						SDL_Log("Window %d resized to %dx%d",
							event.window.windowID, event.window.data1,
							event.window.data2);

						int w = event.window.data1;
						int h = event.window.data2;
						gw.windowWidth = w;
						gw.windowHeight = h;
						gw.setViewport(0, 0, w, h);
						renderer.windowWidth = w;
						renderer.windowHeight = h;
						renderer.rescale();
					}
				}
				else if (event.type == SDL_KEYUP) {
					gw.inpHandler.processKeyUp(event.key.keysym.sym);
					if (event.key.keysym.sym == SDLK_ESCAPE)
						gameActive = false;
					break;
				}
			}
			renderFrame++;
			while (ts.accumulatorFull()) {
				ts.accumulator -= 1.0f / ts.gameUpdateFPS;
				updateFrame++;
				printConsoleCounter++;
				// Code to execute every update frame
				gameUpdate(world, *renderer.cam, updateFrame);

				renderer.cam->pos += glm::vec3(camVelocity, 0.f);
				camVelocity *= 0.90;

				if ((printConsoleCounter > FRAMES_BETWEEN_STAT_UPDATES) && !DISABLE_RUNTIME_CONSOLE) { // means the console updates every second
					printConsoleCounter = 0;
					sendConsoleStats(ts, renderer, world);
				}

				updateFPSGauge.update(ts.gameUpdateFPS / 4);


				ts.processFrameStart();
			}


			ts.calculateAlpha();

			processTestInputs(gw.inpHandler, world, *renderer.cam, camVelocity, renderer);



			renderFPSGauge.update(100); // 100 frame long value buffer

			gameRender(renderer, gw, world);

			gw.displayNewFrame();
		}

		//world.logChunks();
		gw.cleanUp();
		SDL_Quit();
		world.removeChunks();
	}
	catch (std::exception excp) {
		std::cout << "Caught Error: " << excp.what();
	}
	return 0;
}


void gameRender(GameRenderer& renderer, GameWindow& gw, ChunkManager& world) {

	renderer.bindScreenFBOAsRenderTarget();
	renderer.setClearColor(glm::vec4(0.8f, 0.8f, 1.0f, 0.0f));
	glEnable(GL_DEPTH_TEST);
	renderer.clearScreen();

	lastChunkDrawnCount = renderer.drawWorld(world, gw);
	renderer.testDraw();

	gw.bind();
	gw.clear();
	glDrawBuffer(GL_BACK);
	glDisable(GL_DEPTH_TEST);
	renderer.drawLighting();
}

void gameUpdate(ChunkManager& world, Camera& cam, int updateFrame) {
	world.autoGen(cam);
	while (world.genFromQueue());
}

void processTestInputs(InputHandler& inp, ChunkManager& world, Camera& cam, glm::vec2& camVelocity, GameRenderer& renderer) {
	float camSpeed = 0.05f;
	if (inp.testKey(SDLK_w)) {
		camVelocity.y += camSpeed;
		cam.updateFrame();

	}
	if (inp.testKey(SDLK_a)) {
		camVelocity.x -= camSpeed;
		cam.updateFrame();

	}
	if (inp.testKey(SDLK_s)) {
		camVelocity.y -= camSpeed;
		cam.updateFrame();

	}
	if (inp.testKey(SDLK_d)) {
		camVelocity.x += camSpeed;
		cam.updateFrame();

	}

	if (inp.testKey(SDLK_q)) {
		cam.tileScale *= 0.992f;
		cam.updateFrame();
	}
	if (inp.testKey(SDLK_e)) {
		cam.tileScale *= 1.01f;
		cam.updateFrame();
	}

	if (inp.testKey(SDLK_1)) {
		world.removeChunks();
	}

	if (inp.testKey(SDLK_2)) {
		world.logChunks();
	}

	if (inp.testKeyDown(SDLK_3)) {
		renderer.swapCameras();
	}
};


void sendConsoleStats(Timestepper& ts, GameRenderer& renderer, ChunkManager& world) {
	system("CLS");
	printf("Current Update FPS - %.2f \n", 1.0f / utils::averageVector(updateFPSGauge.frametimeBuffer));
	printf("Current Draw FPS - %.2f \n", 1.0f / utils::averageVector(renderFPSGauge.frametimeBuffer));
	printf("Cam Position - %.2f, %.2f \n", renderer.cam->pos.x, renderer.cam->pos.y);
	printf("Cam Frame - X range: %.2f, %.2f   Y range: %.2f, %.2f\n", renderer.cam->getFrame().x, renderer.cam->getFrame().z, renderer.cam->getFrame().y, renderer.cam->getFrame().w);
	printf("Screen Dimensions - Width: %i Height: %i\n", renderer.screenWidth, renderer.screenHeight);
	printf("Window Dimensions - Width: %i Height: %i\n", renderer.windowWidth, renderer.windowHeight);
	printf("Chunk Count: %i \n", world.getChunkCount());
	printf("Empty Chunk Count: %i \n", world.getEmptyChunkCount());
	printf("Number of chunks drawn: %i \n", lastChunkDrawnCount);
}

// i am so sorry I do not know a better place to put this
void initGL() {
	std::cout << "Done" << std::endl;
#ifdef LOADLOGGING_ENABLED
	std::cout << "Initializing OpenGL 3.2..." << std::endl;
#endif
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); // Bit depth of 8 bits
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // OpenGL 3.2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


	glewExperimental = GL_TRUE;
	auto init_res = glewInit();
	if (init_res != GLEW_OK)
	{
		std::cout << glewGetErrorString(glewInit()) << std::endl;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
}