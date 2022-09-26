#pragma once
#include <util/ext/glm/glm.hpp>
#include "Framework/Graphics/Pixmap.hpp"
#include <GL/glew.h>
#include "Framework/Graphics/Shader.hpp"
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/Texture.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/DrawStates.hpp"

class Lighting
{
public:
	static Lighting& Get();
	Lighting(const Lighting&) = delete;

	void setDims(uint16_t p_width, uint16_t p_height);
	void updateLightmapTex();
	void draw(DrawSurface& p_target, GLuint p_screenColorTex);

private:
	Lighting();
	~Lighting();
	Lighting(uint16_t p_width, uint16_t p_height);


	glm::vec2 framePos;
	glm::vec2 frameDim;

	Mesh<GLfloat> overlayMesh;
	DrawStates lightingStates;
	std::shared_ptr<Shader> lightingShader;
	Pixmap lightmap;
	Texture lightmapTex;
	// Just so we don't have to reset it every frame
	bool screenColorPointerSet = false;
};

