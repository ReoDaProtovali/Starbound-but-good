#include "GameRenderer.hpp"
#include "GameWindow.hpp"
GameRenderer::GameRenderer(
	unsigned int p_screenWidth,
	unsigned int p_screenHeight,
	unsigned int p_windowWidth,
	unsigned int p_windowHeight) {

	screenWidth = p_screenWidth;
	screenHeight = p_screenHeight;
	windowWidth = p_windowWidth;
	windowHeight = p_windowHeight;

	imageShader = Shader("./Shaders/ImageVS.glsl", "./Shaders/ImageFS.glsl");
	imageShader.setTexUniform("ourTexture", 0);

	res = ResourceLoader();

	cam.pos = glm::vec3(0.0f, 10.0f * CHUNKSIZE, 1.0f);
	cam.tileScale = 16.0f;

	lighting = Lighting((unsigned int)(windowWidth / cam.tileScale), (unsigned int)(windowHeight / cam.tileScale));

	GameRenderer::loadSpriteSheets();
	GameRenderer::initFBO();
}

void GameRenderer::loadSpriteSheets() {
	bool success = true;
	success = GameRenderer::res.load("res/tiles/spritesheet.png", TextureID::TILESHEET_TEXTURE) &&
		GameRenderer::res.load("res/tiles/testsheet.png", TextureID::TESTSPRITESHEET_TEXTURE);
	if (!success) {
		throw std::invalid_argument("One or more file not found");
	}
	tileSheetTexture = GameRenderer::res.getTexture(TextureID::TILESHEET_TEXTURE);
	tileSheet = SpriteSheet(tileSheetTexture, glm::ivec2(8, 8), tileSheetTexture.getPixelCount() / (8 * 8));
	tileSheet.setMode(SheetMode::BASIC);

	objectSheet = SpriteSheet(); // undefined for now

	entitySheet = SpriteSheet(); // undefined for now

}
void GameRenderer::initFBO() {
	glGenFramebuffers(1, &screenFBO);
	glGenRenderbuffers(1, &depthBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

	screenColorTex.changeDimensions(windowWidth, windowHeight); // Allocates the texture
	screenColorTex.setFiltering(GL_LINEAR);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenColorTex.glID, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	glDrawBuffers(1, DrawBuffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameRenderer::bindScreenFBOAsRenderTarget()
{
	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
	glDrawBuffers(1, DrawBuffers);
}

void GameRenderer::rescale()
{
	screenColorTex.changeDimensions(windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenColorTex.glID, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, windowWidth, windowHeight); // rescale depth buffer
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	cam.updateFrame((float)windowWidth, (float)windowHeight);
}

bool GameRenderer::drawChunk(WorldChunk& p_chunk) {
	if (!p_chunk.meshIsCurrent) { p_chunk.generateVBO(tileSheet); };
	glBindVertexArray(p_chunk.tileMesh.VAO);

	imageShader.use();

	// Set the active texture unit to 0, which was set to represent "ourTexture" within the shader
	// this was done with "imageShader.setTexUniform("ourTexture", 0);"
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
	imageShader.setMat4Uniform("model", modelTransform);

	cam.setDimensions((float)windowWidth / (float)windowHeight);

	// Matrix that transforms from global space, to view space, to clip space in one swoop
	glm::mat4 finalTransform = cam.getTransformMat4((float)windowWidth, (float)windowHeight);
	imageShader.setMat4Uniform("transform", finalTransform);

	glDrawArrays(GL_TRIANGLES, 0, p_chunk.getVBOSize());
	glBindVertexArray(0);
	return true;
}

bool GameRenderer::drawSprite(glm::vec2 p_worldPos, Texture p_spriteTex)
{

	return true;
}

bool GameRenderer::drawSprite(float p_spriteX, float p_spriteY, Texture p_spriteTex)
{
	return true;
}

void GameRenderer::drawLighting() {
	lighting.draw(screenColorTex.glID);
}