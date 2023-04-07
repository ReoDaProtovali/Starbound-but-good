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

	cam->pos = glm::vec3(-16.f, 96.f, 32.0f);
	cam->tileScale = 40.f;
	cam->setDimensions(windowWidth, windowHeight);
	worldRenderer.setCamera(cam);

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
	testReoSprite.setPosition(glm::vec3(-16.f, 96.f, 1.f));

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
	cam->updateFrame();
	DrawStates state;
	state.setTransform(currentCamera.lock()->getTransform());
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

	state.setTransform(currentCamera.lock()->getTransform());

	testReoSprite.setOriginRelative(OriginLoc::CENTER);
	testReoSprite.setRotation(testFrame / 50.f);
	//testReoSprite.draw(m_screenFBO, state);

	static Text debugText(videotype, "");
	static DebugStats& db = DebugStats::Get();
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
			<< "Chunk Gens Per Second - " << db.chunkGenCounter;
		db.statUpdate = false;
		db.drawCalls = 0;
		db.chunkGenCounter = 0;
		updateTimer.startStopwatch();
	}

	debugText.setText(infoString.str());
	debugText.draw(glm::vec2(-0.98f, 0.95f), 20, glm::vec3(1.f, 1.f, 1.f), m_screenFBO, true);

	DebugDraw::drawBoxImmediate(cam->getFrame().x, cam->getFrame().y, cam->getFrameDimensions().x, cam->getFrameDimensions().y, glm::vec3(1.f, 0.f, 0.f), m_screenFBO, *currentCamera.lock());
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
