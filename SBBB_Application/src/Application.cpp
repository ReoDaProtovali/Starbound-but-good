#include "Application.hpp"

Application::Application(GameWindow& p_window)
	: gw(p_window),
	renderer(GameRenderer(gw)),
	world(ChunkManager())
{
	gw.setVSync(true);
}

void Application::run()
{
	gameActive = true;
	LOAD_LOG("Main loop running.");

	while (gameActive) {
		ts.processFrameStart();

		renderFrame++;
		while (ts.accumulatorFull()) {
			// drain the accumulator by 1 update frame
			// used to keep the game updating at a steady fps using math n stuff
			ts.drain();

			// Code to execute every update frame
			updateFrame++;

			update();
			// get things like keyboard events, resize events, quit events, and whatnot
			pollEvents();

			renderer.cam->pos += glm::vec3(camVelocity, 0.f);
			camVelocity *= 0.90;

			// keep track of update fps for console
			updateFPSGauge.update(ts.gameUpdateFPS / 4);
			processConsoleStats();

			ts.processFrameStart();
		}

		// Code to execute every render frame

		// for future frame interpolation
		ts.calculateAlpha();

		handleInput();

		// also for console stats
		renderFPSGauge.update(100); // 100 frame long value buffer

		render();

		// push the renderered frame to the window
		gw.displayNewFrame();
	}
}

void Application::update()
{
	world.autoGen(*renderer.cam);
	while (world.genFromQueue());
}


void Application::render()
{
	renderer.bindScreenFBOAsRenderTarget();
	renderer.setClearColor(glm::vec4(0.8f, 0.8f, 1.0f, 0.0f));
	glEnable(GL_DEPTH_TEST);
	renderer.clearScreen();

	lastChunkDrawnCount = renderer.drawWorld(world, gw);
	renderer.testDraw();

	gw.bind();
	gw.clear();
	glDisable(GL_DEPTH_TEST);
	renderer.drawLighting();
}

void Application::processConsoleStats()
{
	printConsoleCounter++;
	if ((printConsoleCounter > FRAMES_BETWEEN_STAT_UPDATES) && !DISABLE_RUNTIME_CONSOLE) { // means the console updates every second
		printConsoleCounter = 0;
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
}

void Application::pollEvents()
{
	static SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_WINDOWEVENT:
			if (!(event.window.event == SDL_WINDOWEVENT_RESIZED)) break;
			printf("Window %d resized to %dx%d\n", event.window.windowID, event.window.data1,event.window.data2);
			resizeWindow(event.window.data1, event.window.data2);
			break;
		case SDL_QUIT:
			gameActive = false;
			break;
		case SDL_KEYDOWN:
			inp.processKeyDown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			inp.processKeyUp(event.key.keysym.sym);
			if (event.key.keysym.sym == SDLK_ESCAPE) gameActive = false;
			break;
		}
	}
}

void Application::resizeWindow(uint16_t p_w, uint16_t p_h)
{
	gw.windowWidth = p_w;
	gw.windowHeight = p_h;
	gw.setViewport(0, 0, p_w, p_h);
	renderer.setViewport(p_w, p_h);
}

void Application::handleInput()
{
	Camera& cam = *renderer.cam;

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

	if (inp.testKeyDown(SDLK_4)) {
		world.regenVBOs();
	}
}

void Application::cleanUp()
{
	gw.cleanUp();
}
