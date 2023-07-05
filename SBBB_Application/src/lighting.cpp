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
	//dynamicLightingFBO.getColorTexRef(0).changeDimensions(50, 50); // just in case
	//dynamicLightingFBO.getColorTexRef(0).setChannels(GL_RGB);
	dynamicLightingFBO.setDimensions(glm::vec2(64, 36));
	dynamicLightingFBO.getColorTexRef(0).setFiltering(GL_LINEAR, GL_LINEAR);

	lightingInfoFBO.setClearColor(glm::vec4(0.f));
	glm::mat4 tmpmat4 = glm::mat4(1.f);
	//m_lightingCombineShader.addMat4Uniform("transform", tmpmat4);
	m_lightingTextureUniformLoc = m_lightingCombineShader.addTexUniform("lightingTexture", 0);
	m_screenTextureUniformLoc = m_lightingCombineShader.addTexUniform("screenTexture", 1);
	m_screenResUniformLoc = m_lightingCombineShader.addVec2Uniform("screenRes", glm::vec2(1.f));
	m_lightingResUniformLoc = m_lightingCombineShader.addVec2Uniform("lightingRes", glm::vec2(1.f));

	m_lightingShader.addMat4Uniform("transform", tmpmat4);
	m_lightingInfoTexUniformLoc = m_lightingShader.addTexUniform("lightingInfoTex", 0);
	m_topLeftTileCoordUniformLoc = m_lightingShader.addVec2Uniform("topLeftTileCoord", glm::vec2(0.f));
	m_tileDimsUniformLoc = m_lightingShader.addVec2Uniform("tileDims", glm::vec2(64.f));
	m_mousePosUniformLoc = m_lightingShader.addVec2Uniform("mousePos", glm::vec2(0.f));
	m_timeUniformLoc = m_lightingShader.addFloatUniform("time", 0.f);
	m_testLightInfoUniformLoc = m_lightingShader.addVec4Uniform("testLightInfo", glm::vec4(0.f));
	lightingSprite.attachShader(&m_lightingShader);
	//m_lightmapTex = Texture(m_lightmap.width, m_lightmap.height, m_lightmap.getData());
	//m_lightmapTex = m_lightingFBO.getColorTex(0);
	//m_lightmapTex.setFiltering(GL_NEAREST, GL_NEAREST);

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

	//m_lightingStates.attachTexture(lightingInfoFBO.getColorTexRef(0));
	Texture& infoTex = lightingInfoFBO.getColorTexRef(0);

	lightingSprite.attachTexture(infoTex);
	m_lightingShader.setTexUniform(m_lightingInfoTexUniformLoc, 0);
	m_lightingShader.setVec2Uniform(m_topLeftTileCoordUniformLoc, glm::vec2(lightingSprite.getPosition().x, lightingSprite.getPosition().y));
	glm::vec2 tileDims = glm::vec2(std::abs(lightingSprite.bounds.wh.x), std::abs(lightingSprite.bounds.wh.y));
	m_lightingShader.setVec2Uniform(m_tileDimsUniformLoc, tileDims);
	m_lightingShader.setFloatUniform(m_timeUniformLoc, float(SDL_GetTicks()) / 1000.f);

	ImGui::Begin("Lighting");
	ImGui::SliderFloat("Resolution Divisor", &dynamicResDivisor, 1.f, 64.f);
	static glm::vec4 lightInfo{1.f, 1.f, 1.f, 0.3f};
	static bool fullbright = false;
	ImGui::Checkbox("Fullbright", &fullbright);
	ImGui::ColorPicker4("Light Color", glm::value_ptr(lightInfo));
	ImGui::End();

	dynamicLightingFBO.setDimensions(p_screenFBO.getViewportWidth() / dynamicResDivisor, p_screenFBO.getViewportHeight() / dynamicResDivisor);
	m_lightingShader.setVec4Uniform(m_testLightInfoUniformLoc, lightInfo);

	m_lightingShader.setVec2Uniform(m_mousePosUniformLoc, p_states.cam->pixelToTileCoordinates(p_mousePos.x, p_mousePos.y));
	dynamicLightingFBO.setClearColor(glm::vec4(0.25f, 0.25f, 0.25f, 1.f));
	dynamicLightingFBO.clear();

	m_lightingShader.use();
	lightingSprite.attachShader(&m_lightingShader);
	if (!fullbright)
		lightingSprite.draw(dynamicLightingFBO, newStates);
	else
		dynamicLightingFBO.clear();
	m_lightingCombineStates.setTexture(0, dynamicLightingFBO.getColorTexRef(0));
	m_lightingCombineShader.setVec2Uniform(m_screenResUniformLoc, glm::vec2(p_gameWindow.getViewportWidth(), p_gameWindow.getViewportHeight()));
	m_lightingCombineShader.setVec2Uniform(m_lightingResUniformLoc, glm::vec2(dynamicLightingFBO.getViewportWidth(), dynamicLightingFBO.getViewportHeight()));
	p_gameWindow.bind();
	// something I realize now is that calling .draw on an FBO does not actually bind the FBO, it just draws to the current bound thing.
	p_gameWindow.draw(m_overlayMesh, GL_TRIANGLES, m_lightingCombineStates);
}
