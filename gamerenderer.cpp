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

	cam.pos = glm::vec3(0.0, 10.0 * CHUNKSIZE, 1.0);
	cam.tileScale = 128.0;

	lighting = Lighting(floorf(windowWidth / cam.tileScale), floorf(windowHeight / cam.tileScale));

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
	Image tileSheetImage = GameRenderer::res.getImage(TextureID::TILESHEET_TEXTURE);
	tileSheet = SpriteSheet(tileSheetImage, glm::ivec2(8, 8), tileSheetImage.pixelCount() / (8 * 8));
	tileSheetTexture = Texture(tileSheetImage);
	//std::cout << tileSheetTexture.ID;
	objectSheet = SpriteSheet(); // undefined for now

	entitySheet = SpriteSheet(); // undefined for now

}
void GameRenderer::initFBO() {
	//std::cout << windowWidth << " " << windowHeight << std::endl;
	glGenFramebuffers(1, &screenFBO);
	glGenTextures(1, &screenColorTex);
	glGenRenderbuffers(1, &depthBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

	glBindTexture(GL_TEXTURE_2D, screenColorTex);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		windowWidth, windowHeight,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenColorTex, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	glDrawBuffers(1, DrawBuffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//std::cout << "Tex id: " << screenColorTex << std::endl;

}

bool GameRenderer::drawChunk(WorldChunk& p_chunk) {
	if (!p_chunk.vboIsCurrent) { p_chunk.generateVBO(tileSheet); };
	glBindVertexArray(p_chunk.VAO);

	imageShader.use(); // Use the shader that draws an image based on texture coordinates

	// Set the active texture unit to 0, which was set to represent "ourTexture" within the shader
	// this was done with "imageShader.setTexUniform("ourTexture", 0);"
	glActiveTexture(GL_TEXTURE0); 
	// Attach the actual texture to the main texture 2d buffer on unit 0
	glBindTexture(GL_TEXTURE_2D, tileSheetTexture.ID);

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

	cam.setDimensions((float)screenWidth / (float)screenHeight);

	// Matrix that transforms from global space, to view space, to clip space in one swoop
	glm::mat4 finalTransform = cam.getTransformMat4((float)screenWidth, (float)screenHeight, (float)windowWidth, (float)windowHeight);
	imageShader.setMat4Uniform("transform", finalTransform);

	//glViewport(0, 0, screenWidth, screenHeight);
	glDrawArrays(GL_TRIANGLES, 0, p_chunk.getVBOSize());
	glBindVertexArray(0);
	return true;
}

void GameRenderer::doLighting() {
	lighting.draw(screenColorTex);
}