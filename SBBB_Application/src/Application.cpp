#include "Application.hpp"
#include <thread>

Application::Application()
{
}


void Application::run()
{
	startClient();

	startServer();

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_RATE_MS));
		pollEvents();
		if (!gameActive) break;
	}
	//std::this_thread::sleep_for(std::chrono::seconds(10));

    client.stop();
	localServer.stop();
	//gameActive = true;
	//LOAD_LOG("Main loop running.");

	//world.genFixed(20, 10);
	//world.startThreads();

	//while (gameActive) {
	//	ts.processFrameStart();

	//	renderFrame++;
	//	while (ts.accumulatorFull()) {
	//		// drain the accumulator by 1 update frame
	//		// used to keep the game updating at a steady fps using math n stuff
	//		ts.drain();

	//		// Code to execute every update frame
	//		updateFrame++;

	//		update();
	//		if (gw.hasChangedFullscreenState()) {
	//			resizeWindow(gw.windowWidth, gw.windowHeight);
	//		}
	//		// get things like keyboard events, resize events, quit events, and whatnot
	//		pollEvents();

	//		renderer.cam->pos += glm::vec3(camVelocity, 0.f);
	//		camVelocity *= 0.90;

	//		// keep track of update fps for console
	//		updateFPSGauge.update(ts.gameUpdateFPS / 4);
	//		processConsoleStats();

	//		ts.processFrameStart();
	//	}

	//	// Code to execute every render frame

	//	// for future frame interpolation
	//	ts.calculateAlpha();

	//	handleInput();


	//	renderer.cam->lastVelocity.x = camVelocity.x;
	//	renderer.cam->lastVelocity.y = camVelocity.y;
	//	renderer.cam->lastVelocity.z = 0;
	//	// also for console stats
	//	renderFPSGauge.update(20); // 20 frame long value buffer

	//	// Interpolation!
	//	renderer.cam->enableInterpolation(); // doesn't have to be done every frame, but here for clarity
	//	renderer.cam->interpolate(ts.alpha);

	//	renderer.cam->updateFrame();
	//	render();

	//	// push the renderered frame to the window
	//	gw.displayNewFrame();
	//}
}

void Application::startClient()
{
	client.start();
}

void Application::startServer()
{
	localServer.start();
}

void Application::processConsoleStats()
{
	//printConsoleCounter++;
	//if ((printConsoleCounter > FRAMES_BETWEEN_STAT_UPDATES) && !DISABLE_RUNTIME_CONSOLE) { // means the console updates every second
	//	printConsoleCounter = 0;
	//	DebugStats& db = DebugStats::Get();
	//	db.updateFPS = 1.0f / utils::averageVector(updateFPSGauge.frametimeBuffer);
	//	db.drawFPS = 1.0f / utils::averageVector(renderFPSGauge.frametimeBuffer);
	//	db.camX = renderer.cam->pos.x;
	//	db.camY = renderer.cam->pos.y;
	//	auto f = renderer.cam->getFrame();
	//	db.camFX1 = f.x;
	//	db.camFY1 = f.y;
	//	db.camFX2 = f.z;
	//	db.camFY2 = f.w;
	//	db.screenW = renderer.screenWidth;
	//	db.screenH = renderer.screenHeight;
	//	db.windowW = renderer.windowWidth;
	//	db.windowH = renderer.windowHeight;
	//	db.chunkCount = world.getChunkCount();
	//	db.emptyChunkCount = world.getEmptyChunkCount();
	//	db.drawnChunkCount = lastChunkDrawnCount;
	//	db.statUpdate = true;
	//}
}

void Application::pollEvents()
{
	static SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_WINDOWEVENT:
			if (!(event.window.event == SDL_WINDOWEVENT_RESIZED)) break;
			printf("Window %u resized to %dx%d\n", event.window.windowID, event.window.data1, event.window.data2);
			if (event.window.windowID != client.getWindowID()) break;
			client.newWidth = event.window.data1;
			client.newHeight = event.window.data2;
			client.flagResize = true;
			break;
		case SDL_QUIT:
			gameActive = false;
			break;
		case SDL_KEYDOWN:
			client.inp.processKeyDown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			client.inp.processKeyUp(event.key.keysym.sym);
			if (event.key.keysym.sym == SDLK_ESCAPE) gameActive = false;
			break;
		}
	}
}

void Application::handleInput()
{
	//Camera& cam = *renderer.cam;

	//float camSpeed = 0.05f;
	//if (inp.testKey(SDLK_w)) {
	//	camVelocity.y += camSpeed;
	//	//cam.updateFrame();

	//}
	//if (inp.testKey(SDLK_a)) {
	//	camVelocity.x -= camSpeed;
	//	//cam.updateFrame();

	//}
	//if (inp.testKey(SDLK_s)) {
	//	camVelocity.y -= camSpeed;
	//	//cam.updateFrame();

	//}
	//if (inp.testKey(SDLK_d)) {
	//	camVelocity.x += camSpeed;
	//	//cam.updateFrame();

	//}

	//if (inp.testKey(SDLK_q)) {
	//	cam.tileScale *= 0.992f;
	//	cam.updateFrame();
	//}
	//if (inp.testKey(SDLK_e)) {
	//	cam.tileScale *= 1.01f;
	//	cam.updateFrame();
	//}

	//if (inp.testKey(SDLK_1)) {
	//	world.removeChunks();
	//}

	//if (inp.testKey(SDLK_2)) {
	//	world.logChunks();
	//}

	//if (inp.testKeyDown(SDLK_3)) {
	//	renderer.swapCameras();
	//}

	//if (inp.testKeyDown(SDLK_4)) {
	//	LOG("Regenerating VBOs!");
	//	world.regenVBOs();
	//}

	//if (inp.testKeyDown(SDLK_5)) {
	//	gw.toggleFullscreen();
	//}
	//if (inp.testKeyDown(SDLK_6)) {
	//	world.flip();
	//}
}

void Application::cleanUp()
{
	//world.stopThreads();
	//gw.cleanUp();
}
