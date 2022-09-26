#include "Lighting.hpp"
#include "GameConstants.hpp"
#include "Framework/Graphics/GlCheck.hpp"

Lighting& Lighting::Get()
{
	static Lighting instance;
	return instance;
}

Lighting::Lighting() :
	framePos(0.f, 0.f),
	frameDim(0.f, 0.f),
	lightingShader(std::make_shared<Shader>("./src/Shaders/LightingVS.glsl", "./src/Shaders/LightingFS.glsl"))
{

	lightingShader->setTexUniform("screenTexture", 0);
	lightingShader->setTexUniform("lightingTexture", 1);

	lightmapTex = Texture(1, 1, lightmap.getData());
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

//Lighting::Lighting(unsigned int p_width, unsigned int p_height) :
//	lightmap(p_width, p_height), 
//	framePos(0.f, 0.f),
//	frameDim(0.f, 0.f)
//{
//
//	// just for testing
//	//lightmap.setPixel(3, 3, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
//	//lightmap.setPixel(4, 3, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
//	//lightmap.setPixel(5, 3, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
//
//	// bind the uniforms to texture units 0 and 1
//
//
//
//
//
//}

Lighting::~Lighting()
{
	// Remove the gl managed objects when we're done using them
	lightmapTex.remove();
}

void Lighting::setDims(uint16_t p_width, uint16_t p_height)
{
	lightmap = Pixmap(p_width, p_height);
	lightmap.fill(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

void Lighting::updateLightmapTex() {
	lightmapTex.subVec4Data(lightmap.getData());
}

void Lighting::draw(DrawSurface& p_target, GLuint p_screenColorTex)
{
	//if (!screenColorPointerSet) {
	//	lightingStates.setTexture(0, p_screenColorTex);
	//	screenColorPointerSet = true;
	//}

	//p_target.draw(overlayMesh, GL_TRIANGLES, lightingStates);

	glCheck(glBindVertexArray(overlayMesh.VAO)); // Bind the vertices of the quad screen overlay
	glCheck(lightingShader->use()); // Use the lighting shader for the subsequent glDrawArrays call

	glCheck(glActiveTexture(GL_TEXTURE0));
	glCheck(glBindTexture(GL_TEXTURE_2D, p_screenColorTex)); // Screen color texture is bound to unit 0

	glCheck(glActiveTexture(GL_TEXTURE1));
	glCheck(glBindTexture(GL_TEXTURE_2D, lightmapTex.glID)); // Lightmap texture is bound to unit 1

	glCheck(glDrawArrays(GL_TRIANGLES, 0, 6)); // 6 vertices in the quad overlay
	
}
