#pragma once
#include <util/ext/glm/glm.hpp>
#include <GL/glew.h>
#include "Framework/Graphics/Shader.hpp"
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/Texture.hpp"
#include "Framework/Graphics/FrameBuffer.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/DrawStates.hpp"
#include "Framework/Graphics/Pixmap.hpp"
#include "Framework/Graphics/Sprite.hpp"

class Lighting
{
public:
	Lighting();
	~Lighting();

	void setTileInfoTex(Texture p_infoTex);
	void setDims(uint16_t p_width, uint16_t p_height);
	void draw(FrameBuffer& p_screenFBO, DrawSurface& p_gameWindow, DrawStates& p_states, glm::vec2 mousePos);

	Sprite lightingSprite{ glm::vec3(0.f, 0.f, -2.f), Rect(0.f, 0.f, 1.f, 1.f) };
	FrameBuffer lightingInfoFBO;
	FrameBuffer dynamicLightingFBO;

	float dynamicResDivisor = 4.f;

	//private:

	glm::vec2 m_framePos;
	glm::vec2 m_frameDim;

	Mesh<GLfloat> m_overlayMesh;
	DrawStates m_lightingCombineStates;
	DrawStates m_lightingStates;

	// It needs to be a pointer to be able to go down the chain
	Shader m_lightingCombineShader;
	GLint m_lightingTextureUniformLoc = 0;
	GLint m_screenTextureUniformLoc = 0;

	Shader m_lightingShader;
	GLint m_lightingInfoTexUniformLoc = 0;
	GLint m_topLeftTileCoordUniformLoc = 0;
	GLint m_tileDimsUniformLoc = 0;
	GLint m_mousePosUniformLoc = 0;
	GLint m_timeUniformLoc = 0;
	GLint m_testLightInfoUniformLoc = 0;


	Texture m_lightingInfoTex;
	// Just so we don't have to reset it every frame
	bool m_screenTexSet = false;
};

