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

class Lighting
{
public:
	static Lighting& Get();
	Lighting(const Lighting&) = delete;

	void setDims(uint16_t p_width, uint16_t p_height);
	void updateLightmapTex();
	void draw(DrawSurface& p_target, FrameBuffer& p_screenFBO);

private:
	Lighting();
	~Lighting();
	Lighting(uint16_t p_width, uint16_t p_height);

	glm::vec2 m_framePos;
	glm::vec2 m_frameDim;

	Mesh<GLfloat> m_overlayMesh;
	DrawStates m_lightingStates;
	// It needs to be a pointer to be able to go down the chain
	std::shared_ptr<Shader> m_lightingShader;
	Pixmap m_lightmap;

	std::shared_ptr<Texture> m_lightmapTex;
	// Just so we don't have to reset it every frame
	bool m_screenTexSet = false;
};

