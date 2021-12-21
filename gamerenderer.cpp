#include "GameRenderer.hpp"
GameRenderer::GameRenderer() {
	imageShader = Shader("./Shaders/ImageVS.glsl", "./Shaders/ImageFS.glsl");
	res = ResourceLoader();

	GameRenderer::loadSpriteSheets();
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
unsigned int GameRenderer::bufferImage(Image& p_image) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p_image.dimensions.x, p_image.dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, p_image.data); // actually put the image data into the texture buffer
	// WARNING:: very picky about if an image in in RGB format or RBGA format. Try to keep them all RGBA with a bit depth of 8
	return texture;
}

bool GameRenderer::drawChunk(WorldChunk& p_chunk, GameWindow& p_window) {
	if (!p_chunk.vboIsCurrent) { p_chunk.generateVBO(tileSheet); };
	imageShader.use();
	glBindVertexArray(p_chunk.VAO);
	if (!tileSheet.hasglTexture) {
		tileSheet.setglTexture(GameRenderer::bufferImage(tileSheet.image));
	}
	glBindTexture(GL_TEXTURE_2D, tileSheet.glTexture);
	glm::mat4 modelTransform = glm::mat4(1.0f);

	// Matrix that transforms from local space to global space
	modelTransform = glm::translate(modelTransform, glm::vec3(p_chunk.worldPos.x * (float)p_chunk.chunkSize, p_chunk.worldPos.y * (float)p_chunk.chunkSize, 0));

	imageShader.setMat4Uniform("model", modelTransform);

	// Matrix that transforms from global space, to view space, to clip space in one swoop
	glm::mat4 finalTransform = cam.getTransformMat4((float)p_window.width, (float)p_window.height);
	imageShader.setMat4Uniform("transform", finalTransform);

	glDrawArrays(GL_TRIANGLES, 0, p_chunk.getVBOSize());
	glBindVertexArray(0);
	return true;
}
