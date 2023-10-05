#include "Lighting.hpp"
#include "GameConstants.hpp"
#include "Framework/Graphics/GlCheck.hpp"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
Lighting::Lighting() :
	m_framePos(0.f, 0.f),
	m_frameDim(0.f, 0.f),
	m_lightingCombineShader("./src/Shaders/LightingCombineVS.glsl", "./src/Shaders/LightingCombineFS.glsl"),
	m_lightingShader("./src/Shaders/LightingVS.glsl", "./src/Shaders/LightingFS.glsl")
{
	dynamicLightingFBO.setDimensions(glm::vec2(64, 36));
	dynamicLightingFBO.getColorTexRef(0).setFiltering(GL_LINEAR, GL_LINEAR);


	lightingInfoFBO.setClearColor(glm::vec4(0.f));
	m_lightingTextureUniformLoc = m_lightingCombineShader.addTexUniform("lightingTexture", 0);
	m_screenTextureUniformLoc = m_lightingCombineShader.addTexUniform("screenTexture", 1);
	m_lightingResUniformLoc = m_lightingCombineShader.addVec2Uniform("lightingRes", glm::vec2(1.f));

	glm::mat4 tmpmat4 = glm::mat4(1.f);
	m_lightingShader.addMat4Uniform("transform", tmpmat4);
	m_lightingInfoTexUniformLoc = m_lightingShader.addTexUniform("lightingInfoTex", 0);

	lightingSprite.attachShader(&m_lightingShader);
	
	// There needs to be two textures set for lighting to draw properly, but we only have one for now
	Texture tmp;
	m_lightingCombineStates.addTexture(tmp);
	m_lightingCombineStates.attachShader(&m_lightingCombineShader);
	//m_lightingStates.addTexture(tmp)
	lightingInfoFBO.useDepth(false);
	lightingInfoFBO.setDimensions(128, 64);
	lightingInfoFBO.getColorTexRef(0).setFiltering(GL_NEAREST, GL_NEAREST);
	lightingInfoFBO.getColorTexRef(0).setWrapping(GL_CLAMP_TO_EDGE);
	m_overlayMesh.addFloatAttrib(3); // Position attrib
	m_overlayMesh.addFloatAttrib(2); // Tex Coord attrib

	dynamicLightingFBO.getColorTexRef(0).setWrapping(GL_CLAMP_TO_EDGE);

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

void Lighting::setTileInfoTex(Texture p_infoTex)
{
	m_lightingInfoTex = p_infoTex;
}

void Lighting::setDims(uint16_t p_width, uint16_t p_height)
{
	if (p_width < 9 || p_height < 9) {
		ERROR_LOG("Invalid Lighting Dims");
		return;
	}
	lightingInfoFBO.setDimensions(p_width / 8, p_height / 8);

}

void Lighting::draw(FrameBuffer& p_screenFBO, DrawSurface& p_gameWindow, DrawStates& p_states, glm::vec2 p_mousePos)
{
	DrawStates newStates(p_states);
	newStates.m_blendMode.enable();
	glDisable(GL_DEPTH_TEST);
	if (!m_screenTexSet) {

		m_lightingCombineStates.setTexture(0, dynamicLightingFBO.getColorTexRef(0));
		// Now that we know which FBO we are drawing to, we can set the texture.
		m_lightingCombineStates.addTexture(p_screenFBO.getColorTexRef(0));
		m_screenTexSet = true;
	}

	Texture& infoTex = lightingInfoFBO.getColorTexRef(0);

	lightingSprite.attachTexture(infoTex);
	m_lightingShader.setTexUniform(m_lightingInfoTexUniformLoc, 0);

	ImGui::Begin("Lighting");
	ImGui::SliderFloat("Resolution Divisor", &dynamicResDivisor, 1.f, 64.f);
	static bool fullbright = false;
	ImGui::Checkbox("Fullbright", &fullbright);
	ImGui::ColorPicker4("Light Color", glm::value_ptr(dynamicUniforms.testLightInfo));
	ImGui::End();

	//for (int i = 0; i < 8; i++) {
	//	float theta = i / 8.f * M_PI * 2.f + SDL_GetTicks() / 2000.f;
	//	float r = (cos(theta * 2) + 1.f) * 20.f;
	//	m_lights.setAt(i, { glm::vec2(cos(theta) * r, 30.f + sin(theta) * r), glm::vec3(i / 256.f, 0.13f, 0.25f), 0.f, 1.f });
	//}

	m_lights.setAt(0, { {0.f, 70.f}, {1.f, 1.f, 1.f}, 0.f, 1.f });

	dynamicUniforms.topLeftTileCoord = { lightingSprite.getPosition().x, lightingSprite.getPosition().y };
	dynamicUniforms.tileDims = { std::abs(lightingSprite.bounds.wh.x), std::abs(lightingSprite.bounds.wh.y) };
	dynamicUniforms.lightCount = 1;

	dynamicLightingFBO.setDimensions(uint32_t(p_screenFBO.getViewportWidth() / dynamicResDivisor), uint32_t(p_screenFBO.getViewportHeight() / dynamicResDivisor));
	dynamicLightingFBO.setClearColor(glm::vec4(0.25f, 0.25f, 0.25f, 1.f));
	dynamicLightingFBO.clear();

	dynamicUniformBlock.setData(&dynamicUniforms);
	m_lightingShader.use();

	lightingSprite.attachShader(&m_lightingShader);
	if (!fullbright)
		lightingSprite.draw(dynamicLightingFBO, newStates);
	else
		dynamicLightingFBO.clear();
	m_lightingCombineStates.setTexture(0, dynamicLightingFBO.getColorTexRef(0));
	m_lightingCombineShader.setVec2Uniform(m_lightingResUniformLoc, glm::vec2(dynamicLightingFBO.getViewportWidth(), dynamicLightingFBO.getViewportHeight()));
	p_gameWindow.bind();
	// something I realize now is that calling .draw on an FBO does not actually bind the FBO, it just draws to the current bound thing.
	p_gameWindow.draw(m_overlayMesh, GL_TRIANGLES, m_lightingCombineStates);
}
