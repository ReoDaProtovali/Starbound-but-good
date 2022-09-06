#include "GameRenderer.hpp"
#include "GameWindow.hpp"

GameRenderer::GameRenderer(
	GameWindow& p_window) {

#ifdef LOADLOGGING_ENABLED
	std::cout << "GameRenderer instantiated..." << std::endl;
#endif
	windowWidth = p_window.width;
	windowHeight = p_window.height;
	screenWidth = p_window.screenWidth;
	screenHeight = p_window.screenHeight;

	imageShader = Shader("./Shaders/ImageVS.glsl", "./Shaders/ImageFS.glsl");
	imageShader.setTexUniform("imageTexture", 0);

	tileShader = Shader("./Shaders/TileVS.glsl", "./Shaders/TileFS.glsl");
	tileShader.setTexUniform("tileSheet", 0);

#ifdef LOADLOGGING_ENABLED
	std::cout << "Creating Resource loader..." << std::endl;
#endif
	res = ResourceLoader();

	cam.pos = glm::vec3(-16.0f, 315.0f, 32.0f);
	cam.tileScale = 16.0f;
	cam.updateFrame((float)windowWidth, (float)windowHeight);

#ifdef LOADLOGGING_ENABLED
	std::cout << "Creating lighting subsystem..." << std::endl;
#endif
	lighting = Lighting((unsigned int)(windowWidth / cam.tileScale), (unsigned int)(windowHeight / cam.tileScale));

	GameRenderer::loadSpriteSheets();
	GameRenderer::initFBO();
}

void GameRenderer::loadSpriteSheets() {
	bool success = true;
#ifdef LOADLOGGING_ENABLED
	std::cout << "Loading renderer resources..." << std::endl;
#endif
	success = res.load("res/tiles/spritesheet.png", TextureID::TILESHEET_TEXTURE) &&
		res.load("res/tiles/testsheet.png", TextureID::TESTSPRITESHEET_TEXTURE) &&
		res.load("res/cameraframe.png", TextureID::CAMERA_FRAME_TEXTURE) &&
		res.load("res/roetest.png", TextureID::REO_TEST);
	if (!success) {
		throw std::invalid_argument("One or more files not found");
	}
	tileSheetTexture = GameRenderer::res.getTexture(TextureID::TILESHEET_TEXTURE);
	tileSheet = SpriteSheet(tileSheetTexture, glm::ivec2(8, 8), tileSheetTexture.getPixelCount() / (8 * 8));
	tileSheet.setMode(SheetMode::BASIC);

	// Testing purposes
	genericSpriteMesh.addFloatAttrib(3);
	genericSpriteMesh.addFloatAttrib(2);
	genericSpriteMesh.pushVertices({
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // vertex 1
		1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // vertex 2
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // vertex 3
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // vertex 4
		1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // vertex 5
		1.0f,  1.0f, 0.0f,  1.0f, 1.0f // vertex 6
		});
	genericSpriteMesh.genVBO();


	objectSheet = SpriteSheet(); // undefined for now

	entitySheet = SpriteSheet(); // undefined for now

}
void GameRenderer::initFBO() {
#ifdef LOADLOGGING_ENABLED
	std::cout << "Initializing screen frame buffer..." << std::endl;
#endif
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
	glm::mat4 finalTransform = cam.getTransformMat4((float)windowWidth, (float)windowHeight);
	tileShader.setMat4Uniform("transform", finalTransform);

	glDrawArrays(GL_TRIANGLES, 0, p_chunk.getVBOSize());
	glBindVertexArray(0);
	return true;
}

void GameRenderer::drawSprite(glm::vec3 p_worldPos, glm::vec2 p_dimensions, Texture p_spriteTex)
{
	glm::vec4 frame = cam.getFrame();

	std::vector<GLfloat> spriteVerts = { // Local coordinates, top left origin (actually, ends up bottom right in world space because Inverted y) (had to flip texture)
		0.0f, 0.0f, 2.0f,                     0.0f, 1.0f, // vertex 1 tl
		p_dimensions.x, 0.0f, 0.0f,           1.0f, 1.0f, // vertex 2 tr
		0.0f, p_dimensions.y, 0.0f,           0.0f, 0.0f, // vertex 3 bl
		0.0f, p_dimensions.y, 0.0f,           0.0f, 0.0f, // vertex 4 bl
		p_dimensions.x, 0.0f, 0.0f,           1.0f, 1.0f, // vertex 5 tr
		p_dimensions.x, p_dimensions.y, 0.0f, 1.0f, 0.0f // vertex 6 br
	};
	genericSpriteMesh.subVBOData(0, spriteVerts.size(), spriteVerts.data());
	glBindVertexArray(genericSpriteMesh.VAO);

	imageShader.use();

	// Set the active texture unit to 0
	// this was done with "imageShader.setTexUniform("imageTexture", 0);"
	glActiveTexture(GL_TEXTURE0);
	// Attach the actual texture to the main GL_TEXTURE_2D buffer on unit 0
	glBindTexture(GL_TEXTURE_2D, p_spriteTex.glID);

	// Matrix that transforms from local space to global space
	glm::mat4 modelTransform = glm::mat4(1.0f); // Identity
	modelTransform =
		glm::translate(modelTransform, glm::vec3(p_worldPos.x, p_worldPos.y, p_worldPos.z));

	imageShader.setMat4Uniform("model", modelTransform);

	// Matrix that transforms from global space, to view space, to clip space in one swoop
	glm::mat4 finalTransform = cam.getTransformMat4((float)windowWidth, (float)windowHeight);
	imageShader.setMat4Uniform("transform", finalTransform);

	glDrawArrays(GL_TRIANGLES, 0, genericSpriteMesh.getTotalVBOSize());
	glBindVertexArray(0);
}

void GameRenderer::drawLighting() {
	lighting.draw(screenColorTex.glID);
}