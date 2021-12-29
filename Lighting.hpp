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
	Lighting(unsigned int p_width, unsigned int p_height) {
		lightmap = Pixmap(p_width, p_height);
		//lightmap.clear();
		lightmap.setPixel(3, 3, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		lightingShader = Shader("./Shaders/LightingVS.glsl", "./Shaders/LightingFS.glsl");
		// bind the uniforms to texture units 0 and 1
		lightingShader.setTexUniform("screenTexture", 0); 
		lightingShader.setTexUniform("lightmapTexture", 1);

		glGenVertexArrays(1, &quadVAO);
		genQuadVBO();
		lightmapTex = Texture(p_width, p_height, lightmap.getData());
		//glm::vec4 testData = *lightmap.getData();
		//std::cout << testData[1].a << std::endl;
	}
	void genQuadVBO();
	void updateLightmapTex();

	void draw(GLuint screenColorTex, int screenWidth, int screenHeight );
private:

	glm::vec2 framePos;
	glm::vec2 frameDim;

	GLuint quadVAO;
	Shader lightingShader;
	Pixmap lightmap;
	Texture lightmapTex;
};

