#include "Lighting.hpp"
#include "GameConstants.hpp"
#include "Framework/Graphics/GlCheck.hpp"

Lighting::Lighting() :
	m_framePos(0.f, 0.f),
	m_frameDim(0.f, 0.f),
	m_lightingCombineShader("./src/Shaders/LightingCombineVS.glsl", "./src/Shaders/LightingCombineFS.glsl")
{
	m_lightingTextureUniformLoc = m_lightingCombineShader.addTexUniform("lightingTexture", 0);
	m_screenTextureUniformLoc = m_lightingCombineShader.addTexUniform("screenTexture", 1);
	
	m_lightmap.resize(64, 36);
	//m_lightmap.fill(glm::vec4(1.f, 1.f, 1.f, 1.f));
	//m_lightmap.setPixel(3, 3, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	//m_lightmap.setPixel(4, 3, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	//m_lightmap.setPixel(5, 3, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	
	//m_lightmapTex = Texture(m_lightmap.width, m_lightmap.height, m_lightmap.getData());
	m_lightingFBO.setDimensions(glm::vec2(64, 36));
	m_lightmapTex = m_lightingFBO.getColorTex(0);
	m_lightmapTex.setFiltering(GL_NEAREST, GL_NEAREST);

	// There needs to be two textures set for lighting to draw properly, but we only have one for now
	m_lightingStates.addTexture(m_lightmapTex);
	m_lightingStates.attachShader(&m_lightingCombineShader);

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

	m_overlayMesh.pushVBOToGPU();

}


Lighting::~Lighting()
{

}

void Lighting::setDims(uint16_t p_width, uint16_t p_height)
{
	//if (p_width == m_lightmap.width && p_height == m_lightmap.height)
	m_lightingFBO.setDimensions(p_width, p_height);
	//m_lightmap.resize(p_width, p_height);
	//m_lightmap.fill(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

void Lighting::updateLightmapTex() {
	m_lightmapTex.subVec4Data(m_lightmap.getData());
}

void Lighting::draw(FrameBuffer& p_screenFBO)
{
	if (!m_screenTexSet) {
		// Now that we know which FBO we are drawing to, we can set the texture.
		Texture tmp = p_screenFBO.getColorTex(0);
		m_lightingStates.addTexture(tmp);
		m_screenTexSet = true;
	}
	// something I realize now is that calling .draw on an FBO does not actually bind the FBO, it just draws to the current bound thing.
	p_screenFBO.draw(m_overlayMesh, GL_TRIANGLES, m_lightingStates);
}
