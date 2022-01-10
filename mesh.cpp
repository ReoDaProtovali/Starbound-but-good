#include "Mesh.hpp"
#include <iostream>
void Mesh::addAttrib(GLuint p_size)
{
	attribCount++;
	totalVertSize += p_size;
	attribSizes.push_back(p_size);
}

void Mesh::pushVertex(std::initializer_list<GLfloat> p_attribs)
{
	verts.insert(verts.end(), p_attribs);
}

void Mesh::genVBO() {
	glBindVertexArray(VAO);
	if (!VBOInitialized) {
		glGenBuffers(1, &VBO);
	}
	else {
		glDeleteBuffers(1, &VBO);
		glGenBuffers(1, &VBO);
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);

	GLint currentOffset = 0;
	for (unsigned int i = 0; i < attribCount; i++) {
		glVertexAttribPointer(i, attribSizes[i], GL_FLOAT, GL_FALSE, sizeof(GLfloat) * totalVertSize, (void*)currentOffset);
		currentOffset += (GLint)(attribSizes[i] * sizeof(GLfloat));
		glEnableVertexAttribArray(i);

		VBOInitialized = true;
	}
}

void Mesh::remove()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	VBOInitialized = false;
	verts.clear();
	verts.swap(verts);
}
