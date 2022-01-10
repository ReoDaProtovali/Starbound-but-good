#pragma once
#include "GL/glew.h"
#include <vector>
#include <initializer_list>

/// A container for managing vertex buffers and VAOs, allows easy attribute assigning and more
class Mesh
{
	std::vector<GLfloat> verts;
	std::vector<size_t> attribSizes;
	unsigned int totalVertSize = 0;
	unsigned int attribCount = 0;

public:
	Mesh() {
		glGenVertexArrays(1, &VAO);
	}
	void reserve(size_t p_reserveAmount) { verts.reserve(p_reserveAmount); }
	void addAttrib(GLuint p_size);
	void pushVertices(std::initializer_list<GLfloat> p_attribs);
	size_t getTotalVBOSize() { return verts.size(); }
	void genVBO();

	void remove();

	GLuint VBO = 0;
	GLuint VAO = 0;
	bool VBOInitialized = false;

};

