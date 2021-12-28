#pragma once
#include "glm/glm.hpp"
#include "Pixmap.hpp"
#include "GL/glew.h"
#include "Shader.hpp"
#include "Vertex.hpp"
class Lighting
{
public:
	Lighting() {
		lightmap = Pixmap(20, 20);
		lightingShader = Shader("./Shaders/LightingVS.glsl", "./Shaders/LightingFS.glsl");
		glGenVertexArrays(1, &quadVAO);
		genQuadVBO();
		initLightmapTex();
	}
	void genQuadVBO();
	void initLightmapTex();
	void updateLightmapTex();

	void draw(GLuint screenColorTex, int screenWidth, int screenHeight );
private:

	glm::vec2 framePos;
	glm::vec2 frameDim;

	GLuint quadVAO;
	Shader lightingShader;
	Pixmap lightmap;
	unsigned int lightmapTex;
};

