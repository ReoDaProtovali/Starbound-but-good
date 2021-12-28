#include "Lighting.hpp"
#include "GameConstants.hpp"
const GLfloat g_quad_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,  1.0f, 1.0f
};

void Lighting::genQuadVBO()
{
	glBindVertexArray(quadVAO);
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	glVertexAttribPointer(attrib_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(attrib_texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(attrib_position);
	glEnableVertexAttribArray(attrib_texCoord);
}

void Lighting::initLightmapTex() {
	glGenTextures(1, &lightmapTex);
	glBindTexture(GL_TEXTURE_2D, lightmapTex); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameDim.x, frameDim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, lightmap.getData()); // actually put the image data into the texture buffer
	// WARNING:: very picky about if an image in in RGB format or RBGA format. Try to keep them all RGBA with a bit depth of 8
}

void Lighting::updateLightmapTex() {
	glBindTexture(GL_TEXTURE_2D, lightmapTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frameDim.x, frameDim.y, GL_RGBA, GL_FLOAT, lightmap.getData());
}

void Lighting::draw(GLuint p_screenColorTex, int p_screenWidth, int p_screenHeight)
{

	glBindVertexArray(quadVAO);
	lightingShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p_screenColorTex);
	if (!lightingShader.texUniformExists(0)) {
		lightingShader.setTexUniform("screenTexture", 0);
	}
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightmapTex);
	if (!lightingShader.texUniformExists(1)) {
		lightingShader.setTexUniform("lightmapTexture", 1);
	}

	glDrawArrays(GL_TRIANGLES, 0, 6);

}
