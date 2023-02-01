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


	cam->pos = glm::vec3(-16.f, 200.f, 32.0f);
	cam->tileScale = 800.f;
	cam->setDimensions(windowWidth, windowHeight);

	currentCamera = cam;

	overviewCam = std::make_shared<Camera>();
	overviewCam->pos = glm::vec3(-16.0f, 0.0f, 32.0f);
	overviewCam->tileScale = 1024.0f;
	overviewCam->setDimensions(windowWidth, windowHeight);

	// Needs to be a shared pointer such that any DrawStates using it are able to safely copy it
	//m_tileShader = std::make_shared<Shader>(".\\src\\Shaders\\TileVS.glsl", ".\\src\\Shaders\\TileFS.glsl");
	m_tileShader = Shader(".\\src\\Shaders\\TileVS.glsl", ".\\src\\Shaders\\TileGS.glsl", ".\\src\\Shaders\\TileFS.glsl");
	m_tileShader.setTexUniform("tileSheet", 0);

	m_worldDrawStates.attachShader(&m_tileShader);

	videotype.setPixelHeight(50);

	loadTextures();

	testReoSprite.attachShader(&gs.imageShader);
	testReoTexture = res.getTexture(TextureID::REO_TEST);
	testReoSprite.attachTexture(testReoTexture);
	testReoSprite.setPosition(glm::vec3(-16.f, 200.f, 1.f));
	cameraFrameSprite.attachShader(&gs.imageShader);
	cameraFrameTexture = res.getTexture(TextureID::CAMERA_FRAME_TEXTURE);
	cameraFrameSprite.attachTexture(cameraFrameTexture);

	//res.loadTileSet(std::filesystem::path(".\\res\\tilesets\\vanilla"));
	res.loadAllTileSets();

	Texture& t = *res.getTileSheetTexture();
	testTileSheet.attachShader(&gs.imageShader);
	testTileSheet.setBounds(Rect(0.f, 0.f, (float)t.width / 40.f, (float)t.height / 40.f));
	testTileSheet.attachTexture(&t);

	//m_screenFBO.setColorAttachments({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });

	LOAD_LOG("Creating lighting subsystem...");

	m_lighting.setDims(5, 5);

	m_screenFBO.setDimensions(windowWidth, windowHeight); // Initializes
}

GameRenderer::~GameRenderer()
{

}

// Will be changed ----------------------------------------------------------
void GameRenderer::loadTextures() {
	//res.loadTexID("./res/tiles/spritesheet.png", TextureID::TILESHEET_TEXTURE);
	res.loadTexID("./res/roetest.png", TextureID::REO_TEST);
	res.loadTexID("./res/cameraframe.png", TextureID::CAMERA_FRAME_TEXTURE);
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

int GameRenderer::drawWorld(ChunkManager& p_world, DrawSurface& p_target)
{
	int drawnChunkCount = 0;
	bool finished = false;
	Texture* tilesheet = res.getTileSheetTexture();
	tilesheet->setFiltering(GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST);

	m_worldDrawStates.attachTexture(tilesheet);

	m_tileShader.setIntUniform(1, tilesheet->height);

	if (currentCamera.lock()->tileScale > 500.f) {
		m_tileShader.setBoolUniform(4, false);
	}
	else {
		m_tileShader.setBoolUniform(4, true);
	}

	m_worldDrawStates.setTransform(currentCamera.lock()->getTransform());
	//cam->updateFrame();
	p_world.updateDrawList(cam->getFrame());
	drawnChunkCount = p_world.drawVisible(m_screenFBO, m_worldDrawStates, m_tileShader);

	return drawnChunkCount;
}


void GameRenderer::drawLighting() {
	m_lighting.draw(m_screenFBO);
}

void GameRenderer::testDraw()
{
	// it's a bit bad to use the depth test function here, but I haven't moved it into the DrawSurface class yet so whatevs
	glDisable(GL_DEPTH_TEST);
	testFrame++;

	// DrawStates just contains everything opengl needs to know in order to draw.
	// No need to set a texture or shader, they have both attached to the testReoSprite object beforehand
	DrawStates state;

	state.setTransform(currentCamera.lock()->getTransform());

	testReoSprite.setOriginRelative(OriginLoc::CENTER);
	testReoSprite.setRotation(testFrame / 50.f);
	testReoSprite.draw(m_screenFBO, state);

	//cameraFrameSprite.setBounds(Rect(0, 0, cam->getFrameDimensions().x, cam->getFrameDimensions().y));
	//cameraFrameSprite.setOriginRelative(OriginLoc::CENTER);
	//cameraFrameSprite.setPosition(glm::vec3(cam->pos.x, cam->pos.y, 0));
	//cameraFrameSprite.draw(m_screenFBO, state);

	testTileSheet.setOriginRelative(OriginLoc::TOP_LEFT);
	testTileSheet.draw(m_screenFBO, state);

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
			<< "Tile Vertex Count Total: " << db.vertCount; 
		db.statUpdate = false;
		db.drawCalls = 0;
		updateTimer.startStopwatch();
	}

	debugText.setText(infoString.str());
	debugText.draw(glm::vec2(-0.98f, 0.95f), 20, glm::vec3(1.f, 1.f, 1.f), m_screenFBO, true);


	glEnable(GL_DEPTH_TEST);
}

void GameRenderer::swapCameras()
{
	cameraToggle = !cameraToggle;
	if (cameraToggle) {
		currentCamera = overviewCam;
	}
	else {
		currentCamera = cam;
	}
}
