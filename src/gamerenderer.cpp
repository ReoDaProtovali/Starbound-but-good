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

	// Will be moved ------------------------------------------------------------
	tileShader = Shader("./src/Shaders/TileVS.glsl", "./src/Shaders/TileFS.glsl");
	tileShader.setTexUniform("tileSheet", 0);
	// --------------------------------------------------------------------------

	cam.pos = glm::vec3(-16.0f, 315.0f, 32.0f);
	cam.tileScale = 16.0f;
	cam.setPixelDimensions(glm::vec2(windowWidth, windowHeight));
	cam.updateFrame();

	loadTextures();
	testReoSprite = Sprite(glm::vec3(-16.0f, 315.0f, 2.0f), Rect(0.f, 0.f, 3.f, 3.f));
	testReoSprite.attachShader(gs.imageShader);
	testReoTexture = res.getTexture(TextureID::REO_TEST);
	testReoSprite.attachTexture(&testReoTexture);

#ifdef LOADLOGGING_ENABLED
	std::cout << "Creating lighting subsystem..." << std::endl;
#endif
	lighting = Lighting((unsigned int)(windowWidth / cam.tileScale), (unsigned int)(windowHeight / cam.tileScale));

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

	tileSheetTexture = Texture(res.getTexture(TextureID::TILESHEET_TEXTURE));
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

// Will be moved ------------------------------------------------------------
bool GameRenderer::drawChunk(WorldChunk& p_chunk) {
	if (!p_chunk.meshIsCurrent) { p_chunk.generateVBO(); };
	glBindVertexArray(p_chunk.tileMesh.VAO);
	tileShader.use();

	// Set the active texture unit to 0, which was set to represent "tileSheet" within the shader
	// this was done with "tileShader.setTexUniform("tileSheet", 0);"
	glActiveTexture(GL_TEXTURE0);
	// Attach the actual texture to the main GL_TEXTURE_2D buffer on unit 0
	glBindTexture(GL_TEXTURE_2D, tileSheetTexture.glID);

	// Matrix that transforms from local space to global space
	glm::mat4 modelTransform = glm::mat4(1.0f); // Identity
	modelTransform =
		glm::translate(modelTransform,
			glm::vec3(
				p_chunk.worldPos.x * (float)p_chunk.chunkSize,
				p_chunk.worldPos.y * (float)p_chunk.chunkSize,
				0
			)
		);
	tileShader.setMat4Uniform("model", modelTransform);


	// Matrix that transforms from global space, to view space, to clip space in one swoop
	glm::mat4 finalTransform = cam.getTransform();
	tileShader.setMat4Uniform("transform", finalTransform);

	glDrawArrays(GL_TRIANGLES, 0, p_chunk.getVBOSize());
	glBindVertexArray(0);
	return true;
}
// --------------------------------------------------------------------------


void GameRenderer::drawLighting() {
	lighting.draw(screenFBO.getColorTexID(0));
}

void GameRenderer::testDraw()
{
	// it's a bit bad to use the depth test function here, but I haven't moved it into the DrawSurface class yet so whatevs
	glDisable(GL_DEPTH_TEST);
	testFrame++;

	// DrawStates just contains everything opengl needs to know in order to draw.
	// No need to set a texture or shader, they have both attached to the testReoSprite object beforehand
	DrawStates state;

	glm::vec3 camPos = cam.pos;

	testReoSprite.setOriginRelative(OriginLoc::CENTER);

	for (float theta = (float)M_PI * 2.f; theta > 0.f; theta -= (float)M_PI / 64.f) {
		// cool lil polar function
		float r = 8.f + 8.f * std::cos(2.f * theta * testFrame / 400.0f + testFrame / 160.f);

		float x = std::cos(theta) * r;
		float y = std::sin(theta) * r;

		testReoSprite.setPosition(glm::vec3(x + cam.pos.x, y + cam.pos.y, 2.f));
		testReoSprite.setRotation(theta * 3.f + testFrame / 40.f);

		testReoSprite.calculateTransform();
		state.setTransform(cam.getTransform() * testReoSprite.getTransform());
		testReoSprite.draw(screenFBO, state);
	}
	glEnable(GL_DEPTH_TEST);
}
