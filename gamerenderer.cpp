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
	res = ResourceLoader();
	cam.pos = glm::vec3(0.0, 10.0 * CHUNKSIZE, 1.0);
	cam.tileScale = 128.0;

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

	objectSheet = SpriteSheet(); // undefined for now

	entitySheet = SpriteSheet(); // undefined for now

}
void GameRenderer::initFBO() {
	std::cout << "FBO init done" << std::endl;
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
}
void GameRenderer::bindImage(Image& p_image, GLenum p_activeTexture) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(p_activeTexture);
	glBindTexture(GL_TEXTURE_2D, texture); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p_image.dimensions.x, p_image.dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, p_image.data); // actually put the image data into the texture buffer
	// WARNING:: very picky about if an image in in RGB format or RBGA format. Try to keep them all RGBA with a bit depth of 8
}

bool GameRenderer::drawChunk(WorldChunk& p_chunk, GameWindow& p_window) {
	if (!p_chunk.vboIsCurrent) { p_chunk.generateVBO(tileSheet); };
	glBindVertexArray(p_chunk.VAO);

	imageShader.use();
	if (!imageShader.texUniformExists(0)) {
		imageShader.setTexUniform("ourTexture", 0);
		bindImage(tileSheet.image, GL_TEXTURE0);
	}


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
	lighting.draw(screenColorTex, screenWidth, screenHeight);
}