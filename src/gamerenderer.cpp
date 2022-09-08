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

	imageShader = Shader("./src/Shaders/ImageVS.glsl", "./src/Shaders/ImageFS.glsl");
	imageShader.setTexUniform("imageTexture", 0);

	tileShader = Shader("./src/Shaders/TileVS.glsl", "./src/Shaders/TileFS.glsl");
	tileShader.setTexUniform("tileSheet", 0);

#ifdef LOADLOGGING_ENABLED
	std::cout << "Creating Resource loader..." << std::endl;
#endif
	res = ResourceLoader();
	res.load("./res/tiles/spritesheet.png", TextureID::TILESHEET_TEXTURE);
	res.load("./res/roetest.png", TextureID::REO_TEST);
	res.load("./res/cameraframe.png", TextureID::CAMERA_FRAME_TEXTURE);

	tileSheetTexture = Texture(res.getTexture(TextureID::TILESHEET_TEXTURE));
	//tileSheet.setTexture();

	cam.pos = glm::vec3(-16.0f, 315.0f, 32.0f);
	cam.tileScale = 16.0f;
	cam.setPixelDimensions(glm::vec2(windowWidth, windowHeight));
	cam.updateFrame();

#ifdef LOADLOGGING_ENABLED
	std::cout << "Creating lighting subsystem..." << std::endl;
#endif
	lighting = Lighting((unsigned int)(windowWidth / cam.tileScale), (unsigned int)(windowHeight / cam.tileScale));

	//// Testing purposes
	//genericSpriteMesh.addFloatAttrib(3);
	//genericSpriteMesh.addFloatAttrib(2);
	//genericSpriteMesh.pushVertices({
	//	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // vertex 1
	//	1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // vertex 2
	//	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // vertex 3
	//	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // vertex 4
	//	1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // vertex 5
	//	1.0f,  1.0f, 0.0f,  1.0f, 1.0f // vertex 6
	//	});
	//genericSpriteMesh.genVBO();

	screenFBO.setDimensions(windowWidth, windowHeight); // Initializes

}

void GameRenderer::bindScreenFBOAsRenderTarget()
{
	screenFBO.bindAsRenderTarget();
}

void GameRenderer::rescale()
{
	screenFBO.setDimensions(windowWidth, windowHeight);
	cam.setPixelDimensions(glm::vec2((float)windowWidth, (float)windowHeight));
	cam.updateFrame();
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
	glm::mat4 finalTransform = cam.getTransformMat4();
	tileShader.setMat4Uniform("transform", finalTransform);

	glDrawArrays(GL_TRIANGLES, 0, p_chunk.getVBOSize());
	glBindVertexArray(0);
	return true;
}

void GameRenderer::drawSprite(glm::vec3 p_worldPos, glm::vec2 p_dimensions, Texture p_spriteTex)
{

}

void GameRenderer::drawLighting() {
	lighting.draw(screenFBO.getColorTexID());
}