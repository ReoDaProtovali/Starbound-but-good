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
	Lighting() {
		frameDim = glm::vec2(0, 0);
		framePos = glm::vec2(0, 0);
	}
	Lighting(unsigned int p_width, unsigned int p_height);
	void updateLightmapTex();

	void draw(DrawSurface& p_target, GLuint p_screenColorTex);
private:

	glm::vec2 framePos;
	glm::vec2 frameDim;

	Mesh<GLfloat> overlayMesh;
	DrawStates lightingStates;
	Shader lightingShader;
	Pixmap lightmap;
	Texture lightmapTex;
	// Just so we don't have to reset it every frame
	bool screenColorPointerSet = false;
};

