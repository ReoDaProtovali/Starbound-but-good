#include "Lighting.hpp"
#include "GameConstants.hpp"
#include "Framework/Graphics/GlCheck.hpp"

Lighting& Lighting::Get()
{
	static Lighting instance;
	return instance;
}

Lighting::Lighting() :
	m_framePos(0.f, 0.f),
	m_frameDim(0.f, 0.f),
	m_lightingShader(std::make_shared<Shader>("./src/Shaders/LightingVS.glsl", "./src/Shaders/LightingFS.glsl"))
{

	m_lightingShader->setTexUniform("screenTexture", 0);
	m_lightingShader->setTexUniform("lightingTexture", 1);

	m_lightmapTex = Texture(1, 1, m_lightmap.getData());
	m_lightmapTex.setFiltering(GL_LINEAR);

	m_overlayMesh.addFloatAttrib(3); // Position attrib
	m_overlayMesh.addFloatAttrib(2); // Tex Coord attrib

	m_overlayMesh.pushVertices({ // pushing the whole quad vertex array in one swoop
	-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, // vertex 1
	1.0f, -1.0f, 1.0f,  1.0f, 0.0f, // vertex 2
	-1.0f,  1.0f, 1.0f, 0.0f, 1.0f, // vertex 3
	-1.0f,  1.0f, 1.0f, 0.0f, 1.0f, // vertex 4
	1.0f, -1.0f, 1.0f,  1.0f, 0.0f, // vertex 5
	1.0f,  1.0f, 1.0f,  1.0f, 1.0f // vertex 6
		});

	m_overlayMesh.genVBO();

}

//Lighting::Lighting(uint32_t p_width, uint32_t p_height) :
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
	m_lightmapTex.remove();
}

void Lighting::setDims(uint16_t p_width, uint16_t p_height)
{
	m_lightmap = Pixmap(p_width, p_height);
	m_lightmap.fill(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

void Lighting::updateLightmapTex() {
	m_lightmapTex.subVec4Data(m_lightmap.getData());
}

void Lighting::draw(DrawSurface& p_target, GLuint p_screenColorTex)
{
	//if (!screenColorPointerSet) {
	//	lightingStates.setTexture(0, p_screenColorTex);
	//	screenColorPointerSet = true;
	//}

	//p_target.draw(overlayMesh, GL_TRIANGLES, lightingStates);

	glCheck(glBindVertexArray(m_overlayMesh.VAO->ID)); // Bind the vertices of the quad screen overlay
	glCheck(m_lightingShader->use()); // Use the lighting shader for the subsequent glDrawArrays call

	glCheck(glActiveTexture(GL_TEXTURE0));
	glCheck(glBindTexture(GL_TEXTURE_2D, p_screenColorTex)); // Screen color texture is bound to unit 0

	glCheck(glActiveTexture(GL_TEXTURE1));
	glCheck(glBindTexture(GL_TEXTURE_2D, m_lightmapTex.glID)); // Lightmap texture is bound to unit 1

	glCheck(glDrawArrays(GL_TRIANGLES, 0, 6)); // 6 vertices in the quad overlay
	
}
