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

Lighting::Lighting(unsigned int p_width, unsigned int p_height) {
	lightmap = Pixmap(p_width, p_height);

	// just for testing
	lightmap.fill(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
	lightmap.setPixel(3, 3, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	lightmap.setPixel(4, 3, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	lightmap.setPixel(5, 3, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	lightingShader = Shader("./Shaders/LightingVS.glsl", "./Shaders/LightingFS.glsl");
	// bind the uniforms to texture units 0 and 1
	lightingShader.setTexUniform("screenTexture", 0);
	lightingShader.setTexUniform("lightingTexture", 1);

	glGenVertexArrays(1, &quadVAO);
	genQuadVBO();
	lightmapTex = Texture(p_width, p_height, lightmap.getData());
	lightmapTex.setFiltering(GL_LINEAR);
	//glm::vec4 testData = *lightmap.getData();
	//std::cout << testData[1].a << std::endl;
}
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

void Lighting::updateLightmapTex() {
	lightmapTex.subVec4Data(lightmap.getData());
}

void Lighting::draw(GLuint p_screenColorTex)
{
	//std::cout << p_screenColorTex << std::endl;
	//std::cout << lightmapTex.ID << std::endl;
	glBindVertexArray(quadVAO);
	lightingShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p_screenColorTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightmapTex.ID);


	glDrawArrays(GL_TRIANGLES, 0, 6);

}