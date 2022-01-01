#pragma once
#include "glm/glm.hpp"
#include "Pixmap.hpp"
#include "GL/glew.h"
#include "Shader.hpp"
#include "Vertex.hpp"
#include "Texture.hpp"

class Lighting
{
public:
	Lighting() {
		frameDim = glm::vec2(0, 0);
		framePos = glm::vec2(0, 0);
		quadVAO = 0;
	}
	Lighting(unsigned int p_width, unsigned int p_height);
	void genQuadVBO();
	void updateLightmapTex();

	void draw(GLuint p_screenColorTex);
private:

	glm::vec2 framePos;
	glm::vec2 frameDim;

	GLuint quadVAO;
	Shader lightingShader;
	Pixmap lightmap;
	Texture lightmapTex;
};

