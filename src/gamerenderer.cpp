#include "GameRenderer.hpp"
#include "Framework/Window/GameWindow.hpp"

GameRenderer::GameRenderer(
	GameWindow& p_window) {

#ifdef LOADLOGGING_ENABLED
	std::cout << "GameRenderer instantiated..." << std::endl;
#endif
	windowWidth = p_window.width;
	windowHeight = p_window.height;
	screenWidth = p_window.screenWidth;
	screenHeight = p_window.screenHeight;

	gs.init();

	cam.pos = glm::vec3(-16.0f, 315.0f, 32.0f);
	cam.tileScale = 1024.0f;
	cam.setPixelDimensions(glm::vec2(windowWidth, windowHeight));
	cam.updateFrame();

	loadTextures();
	testReoSprite = Sprite(glm::vec3(-16.0f, 315.0f, 2.0f), Rect(0.f, 0.f, 3.f, 3.f));
	testReoSprite.attachShader(gs.imageShader);
	testReoTexture = res.getTexture(TextureID::REO_TEST);
	testReoSprite.attachTexture(testReoTexture);

	tileShader = std::make_shared<Shader>(Shader("./src/Shaders/TileVS.glsl", "./src/Shaders/TileFS.glsl"));
	tileShader->setTexUniform("tileSheet", 0);
	worldDrawStates.attachShader(tileShader);


#ifdef LOADLOGGING_ENABLED
	std::cout << "Creating lighting subsystem..." << std::endl;
#endif
	lighting = Lighting((unsigned int)(5), (unsigned int)(5));

	screenFBO.setDimensions(windowWidth, windowHeight); // Initializes

}

// Will be changed ----------------------------------------------------------
void GameRenderer::loadTextures() {
#ifdef LOADLOGGING_ENABLED
	std::cout << "Creating Resource loader..." << std::endl;
#endif
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

void GameRenderer::rescale()
{
	screenFBO.setDimensions(windowWidth, windowHeight);
	cam.setPixelDimensions(glm::vec2((float)windowWidth, (float)windowHeight));
	cam.updateFrame();
}

int GameRenderer::drawWorld(ChunkManager& p_world, DrawSurface& p_target)
{
	int drawnChunkCount = 0; 
	bool finished = false;
	while (!finished) {
		std::weak_ptr<WorldChunk> weakChunk = p_world.fetchFromFrame(cam.getFrame(), finished);
		if (auto drawChunk = weakChunk.lock()) {
			if (!(drawChunk->invalid || drawChunk->isEmpty)) {
				drawnChunkCount++;
				//printf("Chunk In Frame at: x-%i y-%i \n", drawChunk.worldPos.x, drawChunk.worldPos.y);
				if (!drawChunk->meshIsCurrent) drawChunk->generateVBO();
				worldDrawStates.setTransform(cam.getTransform());
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

	testReoSprite.setOriginRelative(OriginLoc::CENTER);

	state.setTransform(cam.getTransform());

	testReoSprite.setRotation(testFrame / 50.f);
	testReoSprite.draw(screenFBO, state);

	glEnable(GL_DEPTH_TEST);
}
