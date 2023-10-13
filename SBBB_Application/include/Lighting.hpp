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
#include "Framework/Graphics/UniformArray.hpp"
#include "Framework/Graphics/UniformBlock.hpp"

struct Light {
	Light(glm::vec2 p_pos, glm::vec3 p_col, float p_angle, float p_spread) :
		pos(p_pos), col(p_col), angle(p_angle), spread(p_spread) {}
	glm::vec3 col;
	uint8_t padding1[4] = { 0 }; // 16 byte aligned 
	glm::vec2 pos; // 8 byte aligned
	float angle; // 4 byte aligned 
	float spread; // 4 byte aligned
	int type;
	uint8_t padding2[12] = { 0 }; // 16 byte aligned 
};

struct DynamicLightingInfo {
	glm::vec2 topLeftTileCoord{0};
	glm::vec2 tileDims{0};
	glm::vec4 testLightInfo{0};
	GLuint lightCount = 0;
	float padding[3];
};

class Lighting
{
public:
	Lighting();
	~Lighting();

	void setTileInfoTex(Texture p_infoTex);
	void setDims(uint16_t p_width, uint16_t p_height);

	void calculateAmbient();
	void calculateDynamic(FrameBuffer& p_screenFBO);
	void draw(FrameBuffer& p_screenFBO, DrawSurface& p_gameWindow, DrawStates& p_states);

	float dynamicResDivisor = 4.f;

	friend class WorldRenderer;
private:

	void floodAmbient(Pixmap& lightCanvas, Pixmap& infoCanvas);
	Sprite lightingSprite{ glm::vec3(0.f, 0.f, -2.f), Rect(0.f, 0.f, 1.f, 1.f) };
	FrameBuffer lightingInfoFBO;
	FrameBuffer dynamicLightingFBO;
	glm::vec2 m_framePos;
	glm::vec2 m_frameDim;

	Mesh<GLfloat> m_overlayMesh;
	DrawStates m_lightingCombineStates;
	DrawStates m_lightingStates;

	Shader m_lightingCombineShader;
	GLint m_lightingTextureUniformLoc = 0;
	GLint m_ambientLightingTextureUniformLoc = 0;
	GLint m_screenTextureUniformLoc = 0;
	GLint m_lightingResUniformLoc = 0;

	UniformArray<Light> m_lights{1, 64};

	Shader m_dynamicLightingShader;
	DynamicLightingInfo dynamicUniforms;
	UniformBlock<DynamicLightingInfo> dynamicUniformBlock{2};

	Pixmap ambientMap{ 20, 20 };
	Texture ambientTex{};

	GLint m_lightingInfoTexUniformLoc = 0;

	Texture m_lightingInfoTex;
	// Just so we don't have to reset it every frame
	bool m_screenTexSet = false;
};

