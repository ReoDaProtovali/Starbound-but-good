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

void Lighting::updateLightmapTex() {
	lightmapTex.subVec4Data(lightmap.getData());
}

void Lighting::draw(GLuint p_screenColorTex, int p_screenWidth, int p_screenHeight)
{
	//std::cout << p_screenColorTex << std::endl;
	//std::cout << lightmapTex.ID << std::endl;
	glBindVertexArray(quadVAO);
	lightingShader.use();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, p_screenColorTex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 1);


	glDrawArrays(GL_TRIANGLES, 0, 6);

}
