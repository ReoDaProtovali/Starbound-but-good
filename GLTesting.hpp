#pragma once

#ifndef GLTESTING_H
#define GLTESTING_H

#include <stdio.h>

#define GL_GLEXT_PROTOTYPES

#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>
#include "utils.hpp"
#include "glm/glm.hpp"
#include "GameConstants.hpp"
#include "Shader.hpp"


// this is probably illegal in some courts of law
#define vec3 glm::vec3
#define vec2 glm::vec2


void handleVertexAttrBuffers(GLuint& p_vao, std::vector<Vertex>& p_vertices, std::vector<GLuint>& p_indices) {

	glGenVertexArrays(1, &p_vao);
	glBindVertexArray(p_vao);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, p_vertices.size() * sizeof(Vertex), p_vertices.data(), GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_indices.size() * sizeof(GLuint), p_indices.data(), GL_STATIC_DRAW);

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