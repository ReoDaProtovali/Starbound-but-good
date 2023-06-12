#include "Game States/ClientWorldState.hpp"

void ClientWorldState::testDraw()
{

	renderer.cam->updateFrame();
	// it's a bit bad to use the depth test function here, but I haven't moved it into the DrawSurface class yet so whatevs
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	testFrame++;

	renderer.screenFBO.bind();
	// DrawStates just contains everything opengl needs to know in order to draw.
	// No need to set a texture or shader, they have both attached to the testReoSprite object beforehand
	DrawStates state;


	static glm::vec2 mousePos = glm::vec2(0.f);
	static bool lMouseDown = false;
	static bool rMouseDown = false;
	while (auto opt = m_mouseObserver.observe()) {
		MouseEvent m = opt.value();
		mousePos.x = m.x;
		mousePos.y = m.y;
		lMouseDown = bool(m.mouseState & SDL_BUTTON_LMASK);
		rMouseDown = bool(m.mouseState & SDL_BUTTON_RMASK);

	}
	if (playerCam) {
		if (lMouseDown) {
			int spriteIndex = res.getTileInfo("vanilla:richstone").value().get().spriteIndex;
			glm::vec2 tilePos = playerCam->pixelToTileCoordinates(mousePos.x, mousePos.y) - glm::vec2(1.0f, -0.5f);

			m_tileRequester.notifyAll(TileUpdateRequest{ (int)tilePos.x, (int)tilePos.y, 3, spriteIndex });
		}

		if (rMouseDown) {
			glm::vec2 tilePos = playerCam->pixelToTileCoordinates(mousePos.x, mousePos.y) - glm::vec2(1.0f, -0.5f);
			m_tileRequester.notifyAll(TileUpdateRequest{ (int)tilePos.x, (int)tilePos.y, 3, 0 });
		}
	}

	DebugStats& db = globals.debug;

	if (!playerCam) {
		state.setTransform(renderer.cam->getTransform());
	}
	else {

#ifndef DISABLE_DEBUG_STATS
		db.camFX1 = playerCam->getFrame().x;
		db.camFY1 = playerCam->getFrame().y;
		db.camFX2 = playerCam->getFrame().z;
		db.camFY2 = playerCam->getFrame().w;
		db.camX = playerCam->pos.x;
		db.camY = playerCam->pos.y;
#endif // !DISABLE_DEBUG_STATS

		const float INTERP_FACTOR = (float(UPDATE_RATE_FPS) / float(globals.refresh_rate)) / 2.f;
		playerCam->interpolate(INTERP_FACTOR);
		state.setTransform(playerCam->getTransform());
	}

	for (auto& entity : entities) {
		if (playerCam) state.setTransform(playerCam->getTransform());

		if (entity->isPlayer) {
			playerCam = &entity->entityCam;
			//renderer.cam = playerCam; // scuffed

			entity->entityCam.setDimensions(renderer.window.width, renderer.window.height);
			entity->entityCam.setTileScale(renderer.cam->tileScale);
			entity->entityCam.updateFrame();

			// stop jitter
			playerCam->disableInterpolation();
			state.setTransform(playerCam->getTransform());
			playerCam->enableInterpolation();

		}
		entity->draw(renderer.screenFBO, state);
	}

	if (bombCounter < BOMB_COUNTER_MAX) {
		bombCounter--;
		if (bombCounter <= 0) {
			bombCounter = BOMB_COUNTER_MAX;
			bombSprite.setPosition(glm::vec3(33.f)); // behind the camera
			bombSprite.setRotation(bombSprite.getRotation() + 0.02f);
		}
		bombSprite.setScale(glm::vec2(powf(float(bombCounter) / BOMB_COUNTER_MAX - 1, 2.f)) * 30.f);
		bombSprite.setOpacity(powf(float(bombCounter) / BOMB_COUNTER_MAX - 1.f, 2.f));

		bombSprite.setOriginRelative(OriginLoc::CENTER);
		bombSprite.draw(renderer.screenFBO, state);
	}
	static Observer<KeyEvent> keyObserver;
	while (auto opt = keyObserver.observe()) {
		switch (opt.value().keyCode) {
		case SDLK_b:
			bombCounter--;
			if (playerCam)
				bombSprite.setPosition(glm::vec3(playerCam->apparentPos.x, playerCam->apparentPos.y, 2.f));
			break;
		}
	}

	static Text fpsText(DefaultFonts.videotype, "");
	static std::stringstream fpsString("");

	if (testFrame % 20 == 0) {
		fpsString.str("");
		fpsString << "Current draw FPS - " << globals.drawFPS << '\n'
			<< "Current update FPS - " << globals.updateFPS << '\n';
		fpsText.setText(fpsString.str());
	}
	fpsText.draw(glm::vec2(-0.98f, 0.93f), 20, glm::vec3(1.f, 1.f, 1.f), renderer.screenFBO, true);

#ifndef DISABLE_DEBUG_STATS
	static Text debugText(DefaultFonts.videotype, "");
	static fpsGauge updateTimer;

	static std::stringstream infoString("");

	if (db.statUpdate) {
		updateTimer.stopStopwatch();
		infoString.str("");
		infoString
			<< "Camera Position - X: " << std::setprecision(5) << db.camX << " Y: " << db.camY << '\n'
			<< "Camera Frame - Left: " << db.camFX1 << " Right: " << db.camFX2 << " Top: " << db.camFY2 << " Bottom: " << db.camFY1 << '\n'
			<< "Screen Dimensions - " << db.screenW << "x" << db.screenH << '\n'
			<< "Window Dimensions - " << db.windowW << "x" << db.windowH << '\n'
			<< "Chunk Counts - Total: " << db.chunkCount << " Empty: " << db.emptyChunkCount << '\n'
			<< "Drawn Chunk Count - " << db.drawnChunkCount << '\n'
			<< "Noisemap tiles generated - " << db.noisemapTileCount << '\n'
			<< "Draw Calls Per Second - " << db.drawCalls / updateTimer.getSecondsElapsed() << '\n'
			<< "Seconds Since Last Update: " << updateTimer.getSecondsElapsed() << '\n'
			<< "Tile Vertex Count Total: " << db.vertCount << '\n'
			<< "Chunk Gens Per Second - " << db.chunkGenCounter / updateTimer.getSecondsElapsed() << '\n';
		db.statUpdate = false;
		db.drawCalls = 0;
		db.chunkGenCounter = 0;
		updateTimer.startStopwatch();
	}

	debugText.setText(infoString.str());
	debugText.draw(glm::vec2(-0.98f, 0.80f), 20, glm::vec3(1.f, 1.f, 1.f), renderer.screenFBO, true);
#endif

	static Text controlsText(DefaultFonts.videotype, "SBBB alpha v0.0002\nControls -  \nMove: WASD\nZoom: Q and E\nFullscreen: 5\nExplode: B");
	controlsText.setLeftJustification(true);
	controlsText.draw(glm::vec2(0.98f, 0.93f), 20, glm::vec3(1.f, 1.f, 1.f), renderer.screenFBO, true);

	testReoSprite.draw(renderer.screenFBO, state);
	//gui.draw(renderer.screenFBO);
	//testDrawGUI();
	//SBBBDebugDraw::drawBoxImmediate(cam->getFrame().x, cam->getFrame().y, cam->getFrameDimensions().x, cam->getFrameDimensions().y, glm::vec3(1.f, 0.f, 0.f), m_screenFBO, *currentCamera.lock());
	//drawBoxImmediate(tileCam->getFrame().x, tileCam->getFrame().y, tileCam->getFrameDimensions().x, tileCam->getFrameDimensions().y, glm::vec3(0.f, 1.f, 0.f));
}

void ClientWorldState::loadTextures()
{
	res.loadTexID("./res/roetest.png", TextureID::REO_TEST);
	res.loadTexID("./res/cameraframe.png", TextureID::CAMERA_FRAME_TEXTURE);
	res.loadTexID("./res/cameraframe2.png", TextureID::CAMERA_FRAME_TEXTURE2);
	res.loadTexID("./res/me.png", TextureID::ME_TEXTURE);
	res.loadTexID("./res/bomb.png", TextureID::BOMB_TEXTURE);
}

void ClientWorldState::resume()
{
}

void ClientWorldState::close()
{

	gui.removeElement("testbutton");
	gui.removeElement("testtext");
}

void ClientWorldState::init()
{
	// important, opengl NEEDS this
	renderer.window.bindToThisThread();
#ifndef DISABLE_DEBUG_STATS
	globals.debug.drawThread = std::this_thread::get_id();
#endif // DISABLE_DEBUG_STATS

	renderer.cam->pos = glm::vec3(0.f, 0.f, 32.0f);
	renderer.cam->tileScale = 40.f;
	renderer.cam->setDimensions(renderer.window.width, renderer.window.height);
	worldRenderer.setCamera(renderer.cam);

	DefaultFonts.videotype.setPixelHeight(50);

	loadTextures();

	testReoSprite.attachShader(&gs.imageShader);
	testReoTexture = res.getTexture(TextureID::REO_TEST);
	testReoSprite.attachTexture(testReoTexture);
	testReoSprite.setPosition(glm::vec3(-16.f, 102.f, 1.f));
	//testReoSprite.setOpacity(0.5);
	bombSprite.attachTexture(res.getTexture(TextureID::BOMB_TEXTURE));
	bombSprite.setPosition(glm::vec3(33.f));


	testButton.onClick(
		[&](void) {
			testButton.testColor = glm::vec3(0.3f, 0.f, 0.f);
			testButton.disabled = true;
		});
	testButton.onHover(
		[&](bool hovering) {
			if (hovering)
				testButton.testColor = glm::vec3(0.f, 0.4f, 0.f);
			else
				testButton.testColor = glm::vec3(0.f, 0.0f, 0.f);
		});
	testButtonText.disableBackground();
	testButtonText.enableRelativeScaling();
	testButtonText.setTextHeight(0.2f);
	testButtonText.setText("Clickable!");
	testButton.addChild(&testButtonText);


	testText.enableBackground();
	testText.backgroundOpacity = 0.5f;
	testText.setTextHeight(20);
	testText.disableRelativeScaling();
	testText.setText("hello yes why yes hello there :)");
	testText.setScreenBounds(Rect(500, 500, 500, 45));

	gui.addElement(&testText);
	gui.addElement(&testButton);

	LOAD_LOG("Creating lighting subsystem...");

	m_lighting.setDims(5, 5);

	renderer.screenFBO.setDimensions(renderer.window.width, renderer.window.height); // Initializes
}

void ClientWorldState::update()
{
	renderer.screenFBO.bind();
	renderer.setClearColor(glm::vec4(0.2f, 0.2f, 0.3f, 0.0f));
	renderer.clearScreen();

	int drawnChunkCount = drawWorld();

#ifndef DISABLE_DEBUG_STATS
	if (drawnChunkCount != 0) globals.debug.drawnChunkCount = drawnChunkCount;
#endif
	glEnable(GL_DEPTH_TEST);
	testDraw();

	renderer.window.clear();
	renderer.window.bind();
	glDisable(GL_DEPTH_TEST);
	m_lighting.draw(renderer.screenFBO);
}

void ClientWorldState::suspend()
{
}

int ClientWorldState::drawWorld()
{
	DrawStates state;
	if (playerCam) {
		worldRenderer.setCamera(playerCam);
		state.setTransform(playerCam->getTransform());
	}
	else 
	state.setTransform(renderer.cam->getTransform());
	
	return worldRenderer.draw(renderer.screenFBO, state, renderer.window.width);
}
