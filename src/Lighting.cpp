#include "Lighting.hpp"
#include "GameConstants.hpp"

Lighting::Lighting(unsigned int p_width, unsigned int p_height) {
	lightmap = Pixmap(p_width, p_height);

	// just for testing
	lightmap.fill(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	lightmap.setPixel(3, 3, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	lightmap.setPixel(4, 3, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	lightmap.setPixel(5, 3, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	lightingShader = Shader("./src/Shaders/LightingVS.glsl", "./src/Shaders/LightingFS.glsl");
	// bind the uniforms to texture units 0 and 1
	lightingShader.setTexUniform("screenTexture", 0);
	lightingShader.setTexUniform("lightingTexture", 1);


	lightmapTex = Texture(p_width, p_height, lightmap.getData());
	lightmapTex.setFiltering(GL_LINEAR);

	overlayMesh.addFloatAttrib(3); // Position attrib
	overlayMesh.addFloatAttrib(2); // Tex Coord attrib


	overlayMesh.pushVertices({ // pushing the whole quad vertex array in one swoop
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, // vertex 1
		1.0f, -1.0f, 1.0f,  1.0f, 0.0f, // vertex 2
		-1.0f,  1.0f, 1.0f, 0.0f, 1.0f, // vertex 3
		-1.0f,  1.0f, 1.0f, 0.0f, 1.0f, // vertex 4
		1.0f, -1.0f, 1.0f,  1.0f, 0.0f, // vertex 5
		1.0f,  1.0f, 1.0f,  1.0f, 1.0f // vertex 6
		});

	overlayMesh.genVBO();

}

void Lighting::updateLightmapTex() {
	lightmapTex.subVec4Data(lightmap.getData());
}

void Lighting::draw(GLuint p_screenColorTex)
{
	glBindVertexArray(overlayMesh.VAO); // Bind the vertices of the quad screen overlay
	lightingShader.use(); // Use the lighting shader for the subsequent glDrawArrays call

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p_screenColorTex); // Screen color texture is bound to unit 0

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightmapTex.glID); // Lightmap texture is bound to unit 1

	glDrawArrays(GL_TRIANGLES, 0, 6); // 6 vertices in the quad overlay

}
