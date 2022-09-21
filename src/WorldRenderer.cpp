#include "WorldRenderer.hpp"

WorldRenderer::WorldRenderer()
{
	tileShader = Shader("./src/Shaders/TileVS.glsl", "./src/Shaders/TileFS.glsl");
	tileShader.setTexUniform("tileSheet", 0);

	worldDrawStates.attachShader(&tileShader);

	loadTextures();
}

void WorldRenderer::loadTextures()
{
#ifdef LOADLOGGING_ENABLED
	std::cout << "Creating Resource loader for world..." << std::endl;
#endif
	res = ResourceLoader();
	res.load("./res/tiles/spritesheet.png", TextureID::TILESHEET_TEXTURE);

	tileSheetTexture = Texture(res.getTexture(TextureID::TILESHEET_TEXTURE));
	worldDrawStates.attachTexture(&tileSheetTexture);
}

void WorldRenderer::drawChunk(WorldChunk& p_chunk, glm::mat4 p_transform)
{
	if (!p_chunk.meshIsCurrent) { p_chunk.generateVBO(); };
	glBindVertexArray(p_chunk.tileMesh.VAO);
	tileShader.use();

	// Set the active texture unit to 0, which was set to represent "tileSheet" within the shader
	// this was done with "tileShader.setTexUniform("tileSheet", 0);"
	glActiveTexture(GL_TEXTURE0);
	// Attach the actual texture to the main GL_TEXTURE_2D buffer on unit 0
	glBindTexture(GL_TEXTURE_2D, tileSheetTexture.glID);

	// Matrix that transforms from global space, to view space, to clip space in one swoop
	tileShader.setMat4Uniform("transform", p_transform);

	glDrawArrays(GL_TRIANGLES, 0, p_chunk.getVBOSize());
	glBindVertexArray(0);
}
