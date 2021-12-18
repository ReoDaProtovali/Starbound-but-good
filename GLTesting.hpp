#pragma once

#ifndef GLTESTING_H
#define GLTESTING_H

#include <stdio.h>

#define GL_GLEXT_PROTOTYPES

#include <SDL.h>
#include <SDL_opengl.h>
#include "utils.hpp"
#include "glm/glm.hpp"
#include "GameConstants.hpp"
#include "Shader.hpp"


// this is probably illegal in some courts of law
#define vec3 glm::vec3
#define vec2 glm::vec2

struct Vertex {
	Vertex(vec3 p_pos, vec2 p_texCoord) : pos(p_pos), texCoord(p_texCoord) {}
	vec3 pos;
	vec2 texCoord;
};

void handleVertexAttrBuffers(GLuint& vao, int p_width, int p_height) {

	const Vertex vertices1[] = {
		// positions             / / texture coords
		Vertex(vec3(0.5, 0.5, 0.0), vec2(1.0, 1.0)),   // top right
		Vertex(vec3(0.5, -0.5, 0.0), vec2(1.0, 0.0)), // bottom right
		Vertex(vec3(-0.5, -0.5, 0.0), vec2(0.0, 0.0)), // bottom left
		Vertex(vec3(-0.5, 0.5, 0.0), vec2(0.0, 1.0)) // top left
	};
	const Vertex vertices2[] = {
		// positions             / / texture coords
		Vertex(vec3(0.5, 0.5, 0.0), vec2(1.0, 1.0)),   // top right
		Vertex(vec3(0.5, -0.5, 0.0), vec2(1.0, 0.0)), // bottom right
		Vertex(vec3(-0.5, -0.5, 0.0), vec2(0.0, 0.0)), // bottom left
		Vertex(vec3(-0.5, 0.5, 0.0), vec2(0.0, 1.0)) // top left
	};

	const glm::ivec3 indices[] = {  // note that we start from 0!
	glm::ivec3(0, 1, 3),   // first triangle
	glm::ivec3(1, 2, 3)  // second triangle
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(attrib_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(attrib_texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(attrib_position);
	glEnableVertexAttribArray(attrib_texCoord);
}
void bufferImage(TextureID tex, ResourceLoader res) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	Image image = res.getImage(tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.dimensions.x, image.dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data); // actually put the image data into the texture buffer
	// WARNING:: very picky about if an image in in RGB format or RBGA format. Try to keep them all RGBA with a bit depth of 8
}



#endif