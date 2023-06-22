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
	//DrawStates state;


	mousePos = glm::vec2(0.f);
	static bool lMouseDown = false;
	static bool rMouseDown = false;
	while (auto opt = m_mouseObserver.observe()) {
		MouseEvent m = opt.value();
		mousePos.x = m.pixelX;
		mousePos.y = m.pixelY;
		if (m.wasClick) {
			if (m.mouseButton == SDL_BUTTON_LEFT) lMouseDown = true;
			if (m.mouseButton == SDL_BUTTON_RIGHT) rMouseDown = true;
		}
		if (m.wasRelease) {
			if (m.mouseButton == SDL_BUTTON_LEFT) lMouseDown = false;
			if (m.mouseButton == SDL_BUTTON_RIGHT) rMouseDown = false;
		}
		if (lMouseDown && playerCam) {
			int spriteIndex = res.getTileInfo("vanilla:richstone").value().get().spriteIndex;
			glm::vec2 tilePos = playerCam->pixelToTileCoordinates(mousePos.x, mousePos.y);

			m_tileRequester.notifyAll(TileUpdateRequest{ (int)tilePos.x, (int)tilePos.y, 3, spriteIndex });
		}

		if (rMouseDown && playerCam) {
			glm::vec2 tilePos = playerCam->pixelToTileCoordinates(mousePos.x, mousePos.y);
			m_tileRequester.notifyAll(TileUpdateRequest{ (int)tilePos.x, (int)tilePos.y, 3, 0 });
		}
	}

	ImGui::Begin("Toggles");
	if (ImGui::Button("Chunk Borders")) {
		worldRenderer.drawChunkBorders = !worldRenderer.drawChunkBorders;
		worldRenderer.redrawCameraView();
	}
	static bool vsyncOn = true;
	ImGui::Checkbox("Vsync", &vsyncOn);
	
	SDL_GL_SetSwapInterval(vsyncOn);
	
	ImGui::End();
	renderer.screenFBO.bind();
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
		if(Globals::shouldInterpolate()) playerCam->interpolate(INTERP_FACTOR);
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
			if (Globals::shouldInterpolate()) playerCam->disableInterpolation();
			state.setTransform(playerCam->getTransform());
			if (Globals::shouldInterpolate()) playerCam->enableInterpolation();
			state.cam = playerCam;

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
	static Observer<KeyEvent> keyObserver{ globals.keySubject };
	while (auto opt = keyObserver.observe()) {
		switch (opt.value().keyCode) {
		case SDLK_b:
			bombCounter--;
			if (playerCam) {
				if (Globals::shouldInterpolate())
					bombSprite.setPosition(glm::vec3(playerCam->apparentPos.x, playerCam->apparentPos.y, 2.f));
				else
				 bombSprite.setPosition(glm::vec3(playerCam->pos.x, playerCam->pos.y, 2.f));
			}
			break;
		}
	}

	static Text fpsText(DefaultFonts.videotype, "");
	static std::stringstream fpsString("");
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	if (testFrame % 20 == 0) {
		fpsString.str("");
		fpsString << "Current draw FPS - " << io.Framerate << '\n'
			<< "Current update FPS - " << globals.updateFPS << '\n';
		fpsTextField.setText(fpsString.str());
	}
	//fpsText.draw(glm::vec2(-0.98f, 0.93f), 20, glm::vec3(1.f, 1.f, 1.f), renderer.screenFBO, true);

#ifndef DISABLE_DEBUG_STATS
	static Text debugText(DefaultFonts.videotype, "");
	static fpsGauge updateTimer;

	static std::stringstream infoString("");

	if (db.statUpdate) {
		updateTimer.stopStopwatch();
		infoString.str("");
		infoString << fpsString.str()
			<< "Camera Position - X: " << std::setprecision(5) << db.camX << " Y: " << db.camY << '\n'
			<< "Camera Frame - \nLeft: " << db.camFX1 << " Right: " << db.camFX2 << "\n" << "Top: " << db.camFY2 << " Bottom : " << db.camFY1 << '\n'
			<< "Screen Dimensions - " << db.screenW << "x" << db.screenH << '\n'
			<< "Window Dimensions - " << db.windowW << "x" << db.windowH << '\n'
			<< "Chunk Counts - Total: " << db.chunkCount << " Empty: " << db.emptyChunkCount << '\n'
			<< "Drawn Chunk Count - " << db.drawnChunkCount << '\n'
			<< "Noisemap tiles generated - " << db.noisemapTileCount << '\n'
			<< "Draw Calls Per Second - " << db.drawCalls / updateTimer.getSecondsElapsed() << '\n'
			<< "Seconds Since Last Update: " << updateTimer.getSecondsElapsed() << '\n'
			<< "Chunk Gens Per Second - " << db.chunkGenCounter / updateTimer.getSecondsElapsed() << '\n';
		db.statUpdate = false;
		db.drawCalls = 0;
		db.chunkGenCounter = 0;
		updateTimer.startStopwatch();
	}
	ImGui::Begin("Debug Info");
	ImGui::Text(infoString.str().c_str());
	ImGui::End();
	//debugTextField.setText(infoString.str());
	//debugText.draw(glm::vec2(-0.98f, 0.80f), 20, glm::vec3(1.f, 1.f, 1.f), renderer.screenFBO, true);
#endif

	static Text controlsText(DefaultFonts.videotype, "SBBB alpha v0.0002\nControls -  \nMove: WASD\nZoom: Q and E\nFullscreen: 5\nExplode: B");
	controlsText.setLeftJustification(true);
	controlsText.draw(glm::vec2(0.98f, 0.93f), 20, glm::vec3(1.f, 1.f, 1.f), renderer.screenFBO, true);

	//m_worldCam.setDimensions(renderer.window.width, renderer.window.height);
	//SBBBDebugDraw::drawBoxImmediate(playerCam->getFrame().x, playerCam->getFrame().y, playerCam->getFrameDimensions().x, playerCam->getFrameDimensions().y, glm::vec3(0.f, 1.f, 0.f), renderer.screenFBO, m_worldCam);
	//SBBBDebugDraw::drawBoxImmediate(worldRenderer.m_tileCam.getFrame().x, worldRenderer.m_tileCam.getFrame().y, worldRenderer.m_tileCam.getFrameDimensions().x, worldRenderer.m_tileCam.getFrameDimensions().y, glm::vec3(0.f, 1.f, 0.f), renderer.screenFBO, m_worldCam);
	//gui.draw(renderer.screenFBO);
	//testDrawGUI();
	//SBBBDebugDraw::drawBoxImmediate(renderer.cam->getFrame().x, renderer.cam->getFrame().y, renderer.cam->getFrameDimensions().x, renderer.cam->getFrameDimensions().y, glm::vec3(1.f, 0.f, 0.f), m_screenFBO, *currentCamera.lock());
	//SBBBDebugDraw::drawBoxImmediate(tileCam->getFrame().x, tileCam->getFrame().y, tileCam->getFrameDimensions().x, tileCam->getFrameDimensions().y, glm::vec3(0.f, 1.f, 0.f));
}

void ClientWorldState::loadTextures()
{
	res.loadTexID("./res/roetest.png", "reotest");
	res.loadTexID("./res/me.png", "metexture");
	res.loadTexID("./res/bomb.png", "bombtexture");
	res.loadTexID("./res/navmenu.png", "navmenu");

}

void ClientWorldState::resume()
{
}

void ClientWorldState::close()
{

	gui.removeElement("testbutton");
	gui.removeElement("fpstext");
	gui.removeElement("debugtext");
	gui.removeElement("win95box");
	gui.removeElement("tsc");
}

void ClientWorldState::init()
{
	// important, opengl NEEDS this
#ifndef DISABLE_DEBUG_STATS
	globals.debug.drawThread = std::this_thread::get_id();
#endif // DISABLE_DEBUG_STATS

	renderer.cam->pos = glm::vec3(0.f, 0.f, 32.0f);
	renderer.cam->tileScale = 40.f;
	renderer.cam->setDimensions(renderer.window.width, renderer.window.height);
	worldRenderer.setCamera(renderer.cam);

	DefaultFonts.videotype.setPixelHeight(50);

	loadTextures();

	m_worldCam.setDimensions(renderer.window.width, renderer.window.height);
	m_worldCam.setTileScale(500.f);
	bombSprite.attachTexture(res.getTexture("bombtexture"));
	bombSprite.setPosition(glm::vec3(33.f));

	fpsTextField.enableBackground();
	fpsTextField.backgroundOpacity = 0.5f;
	fpsTextField.setTextHeight(18);
	fpsTextField.disableRelativeScaling();
	fpsTextField.setText("No FPS Data");
	fpsTextField.setScreenBounds(Rect(10.f, 10.f, 350.f, 60.f));
	fpsTextField.centered = true;
	fpsTextField.autoScreenWidth = true;
	fpsTextField.autoScreenHeight = true;

	fpsDragBar.setLocalBounds(Rect(0.f, 0.f, 1.f, 0.1f));
	fpsDragBar.backgroundColor = glm::vec3(1.f, 1.f, 1.f);
	fpsDragBar.backgroundOpacity = 0.2f;
	fpsDragBar.setPixelHeight(20);
	fpsDragBar.enableBackground();
	fpsTextField.addChild(&fpsDragBar);

	const float winboxwidth = 200.f;
	win95Box.setScreenBounds(Rect(100.f, 500.f, winboxwidth, 140.f));
	win95Box.useWin95Background();
	win95Box.backgroundOpacity = 0.8f;


	Texture navTex = res.getTexture("navmenu");

	navBarImage.setLocalBounds(Rect(0.0f, 0.0f, 1.f, 0.2f)); // might not be needed
	navBarImage.setPixelWidth(winboxwidth - 6.f);
	navBarImage.setPixelHeight(14.f);
	navBarImage.setPixelOffset(3.f, 5.f);
	navBarImage.setImage(navTex);
	navBarImage.setImageJustification(Corner::TOP_RIGHT);

	//navDragBar.setLocalBounds(Rect(0.0f, 0.0f, 1.f, 0.2f)); // might not be needed
	navDragBar.setPixelOffset(3.f, 3.f);
	navDragBar.setPixelWidth(winboxwidth - 6.f);
	navDragBar.setPixelHeight(16.f);
	navDragBar.enableBackground();
	navDragBar.backgroundColor = glm::vec3(0.f, 0.f, 0.67f);
	win95Box.addChild(&navDragBar);
	win95Box.addChild(&navBarImage);

	//win95BoxText.setPixelOffset(3.f, 16.f);
	//win95BoxText.setLocalBounds(Rect(0.1f, 0.2f, 1.f, 0.2f));
	win95BoxText.setPixelOffset(6.f, 0.f);
	win95BoxText.setPixelHeight(16.f);
	win95BoxText.setPixelWidth(winboxwidth - 6.f);
	win95BoxText.textColor = glm::vec3(1.f);
	win95BoxText.setText("button that cra...");
	win95BoxText.setTextHeight(15.f);
	win95BoxText.centered = false;
	win95Box.addChild(&win95BoxText);

	funnyButtonContainer.setLocalBounds(Rect(0.1, 0.2, 0.8, 0.7));
	funnyButtonContainer.useWin95Background();
	funnyButtonContainer.backgroundOpacity = 0.8f;

	funnyButton.disableBackground();
	funnyButton.onClick([&]() {
			GameStateManager::Get().close();
		});
	funnyButtonContainer.addChild(&funnyButton);
	win95Box.addChild(&funnyButtonContainer);

	tileSheetContainer.setAbsoluteBounds(Rect(0.0f, 0.0f, 0.2f, 0.2f));
	tileSheetContainer.enableBackground();
	tileSheetContainer.backgroundColor = glm::vec3(1.f);
	tileSheetContainer.backgroundOpacity = 1.f;
	Texture tileSheetTexture = res.getTileSheetTexture();
	tileSheetContainer.setImage(tileSheetTexture);

	//gui.addElement(&tileSheetContainer);


	//gui.addElement(&fpsTextField);
	gui.addElement(&win95Box);



	//LOAD_LOG("Creating lighting subsystem...");


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

	if (playerCam)
		worldRenderer.lighting.draw(renderer.screenFBO, renderer.window, worldDrawnState, mousePos);
}

void ClientWorldState::suspend()
{
}

int ClientWorldState::drawWorld()
{
	if (playerCam) {
		worldRenderer.setCamera(playerCam);
		worldDrawnState.setTransform(playerCam->getTransform());
		worldDrawnState.cam = playerCam;
	}
	else {
		worldDrawnState.setTransform(renderer.cam->getTransform());
		worldDrawnState.cam = renderer.cam;
	}
	
	return worldRenderer.draw(renderer.screenFBO, worldDrawnState, renderer.window.width);
}
