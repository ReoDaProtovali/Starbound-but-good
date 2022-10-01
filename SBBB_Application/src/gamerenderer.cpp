#include "GameRenderer.hpp"

GameRenderer::GameRenderer(const GameWindow& p_window) :
	windowWidth(p_window.width),
	windowHeight(p_window.height),
	screenWidth(p_window.screenWidth),
	screenHeight(p_window.screenHeight)
{

	LOAD_LOG("GameRenderer instantiated...");


	cam->pos = glm::vec3(-16.0f, 315.0f, 32.0f);
	cam->tileScale = 128.0f;
	cam->setDimensions(windowWidth, windowHeight);

	currentCamera = cam;

	overviewCam = std::make_shared<Camera>();
	overviewCam->pos = glm::vec3(-16.0f, 315.0f, 32.0f);
	overviewCam->tileScale = 1024.0f;
	overviewCam->setDimensions(windowWidth, windowHeight);

	loadTextures();
	
	//testReoSprite.attachShader(gs.imageShader);
	testReoTexture = res.getTexture(TextureID::REO_TEST);
	//testReoSprite.attachTexture(testReoTexture);

	cameraFrameSprite.attachShader(gs.imageShader);
	cameraFrameTexture = res.getTexture(TextureID::CAMERA_FRAME_TEXTURE);
	cameraFrameSprite.attachTexture(cameraFrameTexture);

	// Needs to be a shared pointer such that any DrawStates using it are able to safely copy it
	tileShader = std::make_shared<Shader>("./src/Shaders/TileVS.glsl", "./src/Shaders/TileFS.glsl");
	tileShader->setTexUniform("tileSheet", 0);
	worldDrawStates.attachShader(tileShader);

	//screenFBO.setColorAttachments({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });


	LOAD_LOG("Creating lighting subsystem...");

	lighting.setDims(5, 5);

	screenFBO.setDimensions(windowWidth, windowHeight); // Initializes

}

GameRenderer::~GameRenderer()
{

}

// Will be changed ----------------------------------------------------------
void GameRenderer::loadTextures() {
	LOAD_LOG("Creating Resource loader...");
	res = ResourceLoader();
	res.load("./res/tiles/spritesheet.png", TextureID::TILESHEET_TEXTURE);
	res.load("./res/roetest.png", TextureID::REO_TEST);
	res.load("./res/cameraframe.png", TextureID::CAMERA_FRAME_TEXTURE);

	tileSheetTexture = res.getTexture(TextureID::TILESHEET_TEXTURE);
	worldDrawStates.attachTexture(tileSheetTexture);

}
// --------------------------------------------------------------------------

void GameRenderer::bindScreenFBOAsRenderTarget()
{
	screenFBO.bind();
}

void GameRenderer::setClearColor(glm::vec4 p_col)
{
	screenFBO.setClearColor(p_col);
}

void GameRenderer::clearScreen()
{
	screenFBO.clear();
}

void GameRenderer::rescale()
{
	screenFBO.setDimensions(windowWidth, windowHeight);
	cam->setDimensions(windowWidth, windowHeight);
	cam->updateFrame();
	overviewCam->setDimensions(windowWidth, windowHeight);
	overviewCam->updateFrame();
}

int GameRenderer::drawWorld(ChunkManager& p_world, DrawSurface& p_target)
{
	int drawnChunkCount = 0;
	bool finished = false;
	while (!finished) {
		std::weak_ptr<WorldChunk> weakChunk = p_world.fetchFromFrame(cam->getFrame(), finished);
		if (auto drawChunk = weakChunk.lock()) {
			if (!(drawChunk->invalid || drawChunk->isEmpty)) {
				drawnChunkCount++;
				if (!drawChunk->meshIsCurrent) drawChunk->generateVBO();
				worldDrawStates.setTransform(currentCamera.lock()->getTransform());
				drawChunk->draw(screenFBO, worldDrawStates);
			}
		}
	}
	return drawnChunkCount;
}


void GameRenderer::drawLighting() {
	lighting.draw(screenFBO, screenFBO.getColorTexID(0));
}

void GameRenderer::testDraw()
{
	// it's a bit bad to use the depth test function here, but I haven't moved it into the DrawSurface class yet so whatevs
	glDisable(GL_DEPTH_TEST);
	testFrame++;

	// DrawStates just contains everything opengl needs to know in order to draw.
	// No need to set a texture or shader, they have both attached to the testReoSprite object beforehand
	DrawStates state;

	state.addTexture(testReoTexture);
	state.attachShader(gs.imageShader);
	state.setTransform(currentCamera.lock()->getTransform());

	testReoSprite.setOriginRelative(OriginLoc::CENTER);
	testReoSprite.setRotation(testFrame / 50.f);
	testReoSprite.draw(screenFBO, state);

	//cameraFrameSprite = Sprite(glm::vec3(cam.pos.x, cam.pos.y, 2.0f), Rect(cam.getFrame().x, cam.getFrame().y, cam.getFrame().z, cam.getFrame().w));
	//cameraFrameSprite.setBounds(Rect(0, 0, cam->getFrameDimensions().x, cam->getFrameDimensions().y));
	//cameraFrameSprite.setOriginRelative(OriginLoc::CENTER);
	//cameraFrameSprite.setPosition(glm::vec3(cam->pos.x, cam->pos.y, 0));
	//cameraFrameSprite.draw(screenFBO, state);

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
