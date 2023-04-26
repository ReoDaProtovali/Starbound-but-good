#include "GameRenderer.hpp"
#include <sstream>
#include <iomanip>

GameRenderer::GameRenderer(const GameWindow& p_window) :
	windowWidth(p_window.windowWidth),
	windowHeight(p_window.windowHeight),
	screenWidth(p_window.screenWidth),
	screenHeight(p_window.screenHeight)
{

	LOAD_LOG("GameRenderer instantiated...");

	cam->pos = glm::vec3(0.f, 0.f, 32.0f);
	cam->tileScale = 40.f;
	cam->setDimensions(windowWidth, windowHeight);
	worldRenderer.setCamera(cam.get());

	currentCamera = cam;

	overviewCam = std::make_shared<Camera>();
	overviewCam->pos = glm::vec3(-16.0f, 0.0f, 32.0f);
	overviewCam->tileScale = 1024.0f;
	overviewCam->setDimensions(windowWidth, windowHeight);

	videotype.setPixelHeight(50);

	loadTextures();

	testReoSprite.attachShader(&gs.imageShader);
	testReoTexture = res.getTexture(TextureID::REO_TEST);
	testReoSprite.attachTexture(testReoTexture);
	testReoSprite.setPosition(glm::vec3(-16.f, 102.f, 1.f));
	//testReoSprite.setOpacity(0.5);
	bombSprite.attachTexture(res.getTexture(TextureID::BOMB_TEXTURE));
	bombSprite.setPosition(glm::vec3(33.f));

	//ground.onSpawn(testb2World);
	//box1.onSpawn(testb2World);
	//box2.onSpawn(testb2World);
	//box3.onSpawn(testb2World);

	LOAD_LOG("Creating lighting subsystem...");

	m_lighting.setDims(5, 5);

	m_screenFBO.setDimensions(windowWidth, windowHeight); // Initializes

}

GameRenderer::~GameRenderer()
{

}

// Will be changed ----------------------------------------------------------
void GameRenderer::loadTextures() {
	res.loadTexID("./res/roetest.png", TextureID::REO_TEST);
	res.loadTexID("./res/cameraframe.png", TextureID::CAMERA_FRAME_TEXTURE);
	res.loadTexID("./res/cameraframe2.png", TextureID::CAMERA_FRAME_TEXTURE2);
	res.loadTexID("./res/me.png", TextureID::ME_TEXTURE);
	res.loadTexID("./res/bomb.png", TextureID::BOMB_TEXTURE);
}
// --------------------------------------------------------------------------

void GameRenderer::bindScreenFBOAsRenderTarget()
{
	m_screenFBO.bind();
}

void GameRenderer::setClearColor(glm::vec4 p_col)
{
	m_screenFBO.setClearColor(p_col);
}

void GameRenderer::clearScreen()
{
	m_screenFBO.clear();
}

void GameRenderer::setViewport(uint16_t p_w, uint16_t p_h)
{
	windowWidth = p_w;
	windowHeight = p_h;
	m_screenFBO.setDimensions(windowWidth, windowHeight);
	cam->setDimensions(windowWidth, windowHeight);
	cam->updateFrame();
	overviewCam->setDimensions(windowWidth, windowHeight);
	overviewCam->updateFrame();
}

int GameRenderer::drawWorld()
{
	DrawStates state;
	if (!playerCam) {
		state.setTransform(currentCamera.lock()->getTransform());
	}
	else {
		playerCam->enableInterpolation();
		state.setTransform(playerCam->getTransform());
	}
	return worldRenderer.draw(m_screenFBO, state, windowWidth);
}

void GameRenderer::drawLighting() {
	m_lighting.draw(m_screenFBO);
}

void GameRenderer::testDraw()
{

	cam->updateFrame();
	// it's a bit bad to use the depth test function here, but I haven't moved it into the DrawSurface class yet so whatevs
	glDisable(GL_DEPTH_TEST);
	testFrame++;

	m_screenFBO.bind();
	// DrawStates just contains everything opengl needs to know in order to draw.
	// No need to set a texture or shader, they have both attached to the testReoSprite object beforehand
	DrawStates state;

	static DebugStats& db = DebugStats::Get();

	static glm::vec2 mousePos = glm::vec2(0.f);
	while (auto opt = m_mouseObserver.observe()) {
		MouseEvent m = opt.value();
		mousePos.x = m.x;
		mousePos.y = m.y;
	}


	if (!playerCam) {
		state.setTransform(currentCamera.lock()->getTransform());
	}
	else {
		db.camFX1 = playerCam->getFrame().x;
		db.camFY1 = playerCam->getFrame().y;
		db.camFX2 = playerCam->getFrame().z;
		db.camFY2 = playerCam->getFrame().w;
		db.camX = playerCam->pos.x;
		db.camY = playerCam->pos.y;

		const float INTERP_FACTOR = (float(UPDATE_RATE_FPS) / float(globals.refresh_rate)) / 2.f;
		playerCam->interpolate(INTERP_FACTOR);
		state.setTransform(playerCam->getTransform());
	}

	for (auto& entity : entities) {
		if (playerCam) state.setTransform(playerCam->getTransform());

		if (entity->isPlayer) {
			//smoothedPlayerPos = utils::lerp(smoothedPlayerPos, glm::vec3(entity->getInterpolatedPosition().x, entity->getInterpolatedPosition().y, cam->pos.z), 0.1f);
			playerCam = &entity->entityCam;
			worldRenderer.setCamera(playerCam);

			entity->entityCam.setDimensions(windowWidth, windowHeight);
			entity->entityCam.setTileScale(cam->tileScale);
			entity->entityCam.updateFrame();

			// stop jitter
			playerCam->disableInterpolation();
			state.setTransform(playerCam->getTransform());
			//auto mouseTileLoc = playerCam->pixelToTileCoordinates(mousePos.x, mousePos.y);
			//testReoSprite.setPosition(glm::vec3(mouseTileLoc.x, mouseTileLoc.y, 5.f));
			//testReoSprite.setOriginRelative(OriginLoc::CENTER);
			//testReoSprite.setRotation(testFrame / 50.f);
			//testReoSprite.draw(m_screenFBO, state);
			playerCam->enableInterpolation();

		}
		entity->draw(m_screenFBO, state);
	}


	static Text debugText(videotype, "");
	static fpsGauge updateTimer;

	static std::stringstream infoString("");
	if (db.statUpdate) {
		updateTimer.stopStopwatch();
		infoString.str("");
		infoString << "Current draw FPS - " << db.drawFPS << '\n'
			<< "Current update FPS - " << db.updateFPS << '\n'
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
	debugText.draw(glm::vec2(-0.98f, 0.93f), 20, glm::vec3(1.f, 1.f, 1.f), m_screenFBO, true);

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
		bombSprite.draw(m_screenFBO, state);
	}
	static Observer<KeyEvent> keyObserver;
	while (auto opt = keyObserver.observe()) {
		switch (opt.value().keyCode) {
		case SDLK_b:
			bombCounter--;
			bombSprite.setPosition(glm::vec3(playerCam->apparentPos.x, playerCam->apparentPos.y, 2.f));
			break;
		}
	}

	static Text controlsText(videotype, "SBBB beta v0.0001\nControls -  \nMove: WASD\nZoom: Q and E\nFullscreen: 5\nExplode: B");
	controlsText.setLeftJustification(true);
	controlsText.draw(glm::vec2(0.98f, 0.93f), 20, glm::vec3(1.f, 1.f, 1.f), m_screenFBO, true);

	//SBBBDebugDraw::drawBoxImmediate(cam->getFrame().x, cam->getFrame().y, cam->getFrameDimensions().x, cam->getFrameDimensions().y, glm::vec3(1.f, 0.f, 0.f), m_screenFBO, *currentCamera.lock());
	//drawBoxImmediate(tileCam->getFrame().x, tileCam->getFrame().y, tileCam->getFrameDimensions().x, tileCam->getFrameDimensions().y, glm::vec3(0.f, 1.f, 0.f));

	glEnable(GL_DEPTH_TEST);
}

void GameRenderer::swapCameras()
{
	static int cameraIndex = 0;
	cameraIndex++;
	cameraIndex %= 3;
	if (cameraIndex == 1) {
		currentCamera = overviewCam;
	}
	else if (cameraIndex == 0) {
		currentCamera = cam;
	}
}
