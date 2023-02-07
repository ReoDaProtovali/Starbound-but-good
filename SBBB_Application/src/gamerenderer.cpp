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

	cam->pos = glm::vec3(123.f, 100.f, 32.0f);
	cam->tileScale = 40.f;
	cam->setDimensions(windowWidth, windowHeight);

	currentCamera = cam;

	overviewCam = std::make_shared<Camera>();
	overviewCam->pos = glm::vec3(-16.0f, 0.0f, 32.0f);
	overviewCam->tileScale = 1024.0f;
	overviewCam->setDimensions(windowWidth, windowHeight);


	tileCam = std::make_shared<Camera>();
	tileCam->pos = glm::vec3(-16.f, 200.f, 32.0f);
	tileCam->tileScale = 40.0f;
	tileCam->disableAutoFrame();

	FBOSprite.attachTexture(m_tileFBO.getColorTex(0));
	FBOSprite.attachShader(&gs.imageShader);

	// Needs to be a shared pointer such that any DrawStates using it are able to safely copy it
	//m_tileShader = std::make_shared<Shader>(".\\src\\Shaders\\TileVS.glsl", ".\\src\\Shaders\\TileFS.glsl");
	m_tileShader = Shader(".\\src\\Shaders\\TileVS.glsl", ".\\src\\Shaders\\TileGS.glsl", ".\\src\\Shaders\\TileFS.glsl");
	m_tileShader.setTexUniform("tileSheet", 0);

	m_tileShader.setBoolUniform(4, true);

	m_tileDrawStates.attachShader(&m_tileShader);

	videotype.setPixelHeight(50);

	loadTextures();

	testReoSprite.attachShader(&gs.imageShader);
	testReoTexture = res.getTexture(TextureID::REO_TEST);
	testReoSprite.attachTexture(testReoTexture);
	testReoSprite.setPosition(glm::vec3(-16.f, 200.f, 1.f));

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

int GameRenderer::drawWorld(ChunkManager& p_world, DrawSurface& p_target)
{
	Texture* tilesheet = res.getTileSheetTexture();
	//tilesheet->setFiltering(GL_NEAREST, GL_NEAREST);

	m_tileDrawStates.attachTexture(tilesheet);

	m_tileShader.setIntUniform(1, tilesheet->height);

	int drawnChunkCount = 0;

	auto f = cam->getFrame();
	f.y -= CHUNKSIZE * 4;
	f.w += CHUNKSIZE;
	f.z += CHUNKSIZE * 2;
	f.x -= CHUNKSIZE * 2;

	static glm::ivec4 chunkFramePrev;
	glm::ivec4 chunkFrame = utils::frameToChunkCoords(f / 2.f) * 2;


	if (chunkFrame == chunkFramePrev) {
		if (!p_world.notifyNewChunk) {
			return 0;
		}
		p_world.notifyNewChunk = false;
	};

	float pixelsPerTile = (float)windowWidth / (float)cam->tileScale;
	float pixelsPerTileMin = std::fminf(pixelsPerTile, 8.f);

	glm::ivec4 chunkFrameTiles = utils::frameToChunkCoords(f / 2.f) * CHUNKSIZE * 2;
	tileCam->setFrame(
		(float)chunkFrameTiles.x, (float)chunkFrameTiles.y, float(chunkFrameTiles.z - chunkFrameTiles.x), float(chunkFrameTiles.w - chunkFrameTiles.y)
	);
	//glEnable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	FBOSprite.setBounds(Rect(0, 0, tileCam->getFrameDimensions().x, tileCam->getFrameDimensions().y));
	FBOSprite.setPosition(glm::vec3(tileCam->getFrame().x, tileCam->getFrame().w, 0));

	m_tileFBO.setDimensions(glm::vec2(pixelsPerTileMin * (chunkFrameTiles.z - chunkFrameTiles.x), pixelsPerTileMin * (chunkFrameTiles.w - chunkFrameTiles.y)));
	//std::cout << pixelsPerTileMin * (chunkFrameTiles.z - chunkFrameTiles.x) << " " << pixelsPerTileMin * (chunkFrameTiles.w - chunkFrameTiles.y) << '\n';
	m_tileDrawStates.setTransform(tileCam->getTransform());
	m_tileFBO.clear();
	if (currentCamera.lock()->tileScale > 500.f) {
		m_tileShader.setBoolUniform(4, false);
	}
	else {
		m_tileShader.setBoolUniform(4, true);
	}
	drawnChunkCount = p_world.drawChunkFrame(chunkFrame.x - 1, chunkFrame.y - 1, chunkFrame.z + 1, chunkFrame.w + 1, m_tileFBO, m_tileDrawStates, m_tileShader);

	chunkFramePrev = chunkFrame;
	return drawnChunkCount;
}
// debug function
void GameRenderer::drawBoxImmediate(float p_x, float p_y, float p_w, float p_h, glm::vec3 p_col) {
	static Mesh<float> s_Mesh{ NO_VAO_INIT };
	static bool firstRun = true;
	if (!s_Mesh.VAOInitialized) {
		s_Mesh.setStreamType(GL_DYNAMIC_DRAW);
		s_Mesh.addFloatAttrib(3);
	}

	s_Mesh.clean();

	// top side
	Rect localBounds = Rect(-0.5f, -0.5f, p_w + 1.f, 1.f);
	auto tl = localBounds.getTL();
	auto tr = localBounds.getTR();
	auto bl = localBounds.getBL();
	auto br = localBounds.getBR();

	s_Mesh.pushVertices({
		tl.x, -tl.y, 0.f,
		tr.x, -tr.y, 0.f,
		bl.x, -bl.y, 0.f,
		bl.x, -bl.y, 0.f,
		tr.x, -tr.y, 0.f,
		br.x, -br.y, 0.f
		});

	// left side
	localBounds = Rect(-0.5f, -0.5f, 1.f, p_h + 1.f);
	tl = localBounds.getTL();
	tr = localBounds.getTR();
	bl = localBounds.getBL();
	br = localBounds.getBR();

	s_Mesh.pushVertices({
		tl.x, -tl.y, 0.f,
		tr.x, -tr.y, 0.f,
		bl.x, -bl.y, 0.f,
		bl.x, -bl.y, 0.f,
		tr.x, -tr.y, 0.f,
		br.x, -br.y, 0.f
		});

	// right side
	localBounds = Rect(p_w - 0.5f, -0.5f, 1.f, p_h + 1.f);
	tl = localBounds.getTL();
	tr = localBounds.getTR();
	bl = localBounds.getBL();
	br = localBounds.getBR();

	s_Mesh.pushVertices({
		tl.x, -tl.y, 0.f,
		tr.x, -tr.y, 0.f,
		bl.x, -bl.y, 0.f,
		bl.x, -bl.y, 0.f,
		tr.x, -tr.y, 0.f,
		br.x, -br.y, 0.f
		});

	// bottom side
	localBounds = Rect(-0.5f, p_h - 0.5f, p_w + 1.f, 1.0f);
	tl = localBounds.getTL();
	tr = localBounds.getTR();
	bl = localBounds.getBL();
	br = localBounds.getBR();

	s_Mesh.pushVertices({
		tl.x, -tl.y, 0.f,
		tr.x, -tr.y, 0.f,
		bl.x, -bl.y, 0.f,
		bl.x, -bl.y, 0.f,
		tr.x, -tr.y, 0.f,
		br.x, -br.y, 0.f
		});

	if (firstRun) {
		s_Mesh.pushVBOToGPU();
	}
	else {
		s_Mesh.subCurrentVBOData();
	}

	DrawStates d;
	d.attachShader(&gs.solidColorShader);
	gs.solidColorShader.setVec3Uniform(1, p_col);
	auto currentTransform = glm::mat4(1);
	currentTransform = glm::translate(currentTransform, glm::vec3(p_x, p_y, 0));
	d.setTransform(currentCamera.lock()->getTransform() * currentTransform);
	m_screenFBO.draw(s_Mesh, GL_TRIANGLES, d);
	firstRun = false;
}

void GameRenderer::drawLighting() {
	m_lighting.draw(m_screenFBO);
}

void GameRenderer::testDraw()
{
	// it's a bit bad to use the depth test function here, but I haven't moved it into the DrawSurface class yet so whatevs
	glDisable(GL_DEPTH_TEST);
	testFrame++;

	m_screenFBO.bind();
	// DrawStates just contains everything opengl needs to know in order to draw.
	// No need to set a texture or shader, they have both attached to the testReoSprite object beforehand
	DrawStates state;

	state.setTransform(currentCamera.lock()->getTransform());

	//static Sprite testFBOsprite(glm::vec3(0, 0, 5), Rect(0, 0, 10, 10));
	//testFBOsprite.setPosition(glm::vec3(cam->pos.x, cam->pos.y, 0));

	FBOSprite.draw(m_screenFBO, state);

	//testReoSprite.setOriginRelative(OriginLoc::CENTER);
	//testReoSprite.setRotation(testFrame / 50.f);
	//testReoSprite.draw(m_screenFBO, state);

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
			<< "Tile Vertex Count Total: " << db.vertCount << '\n'
			<< "Chunk Gens Per Second - " << db.chunkGenCounter;
		db.statUpdate = false;
		db.drawCalls = 0;
		db.chunkGenCounter = 0;
		updateTimer.startStopwatch();
	}

	debugText.setText(infoString.str());
	debugText.draw(glm::vec2(-0.98f, 0.95f), 20, glm::vec3(1.f, 1.f, 1.f), m_screenFBO, true);

	//drawBoxImmediate(cam->getFrame().x, cam->getFrame().y, cam->getFrameDimensions().x, cam->getFrameDimensions().y, glm::vec3(1.f, 0.f, 0.f));
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
	else {
		currentCamera = tileCam;
	}
}
