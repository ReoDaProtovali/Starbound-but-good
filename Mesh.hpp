#pragma once
#include "GL/glew.h"
#include <vector>
#include <initializer_list>



// Holds the attribute schema to allow for arbitrary type and size.
struct Attrib { 
	Attrib(GLuint p_size, GLenum p_type) {
		size = p_size;
		type = p_type;
	}
	GLuint size;
	GLenum type;
};
/// A container for managing vertex buffers and VAOs, allows easy attribute assigning and more
/// Templated for the vertex data being passed in, capable of holding a combination of float/int in a packed struct as well as single types. Only supports float and uint for now.
/// Trust me it's much less complicated if I move all the .cpp code into the .hpp file. shush.
template <class T>
class Mesh
{
	std::vector<T> verts; // Vert data in any form you deem fit.
	
	std::vector<Attrib> attribList; // List of attributes the shader uses.
	unsigned int totalVertSize = 0;

public:
	Mesh<T>() {
		glGenVertexArrays(1, &VAO);
	}

	/// Allows you to skip a few memory allocations if you reserve it right off the bat.
	void reserve(size_t p_reserveAmount) { verts.reserve(p_reserveAmount); }

	/// Size specifies number of floats required for the attribute.
	void addFloatAttrib(GLuint p_size) {
		totalVertSize += sizeof(GLfloat) * p_size;
		attribList.push_back(Attrib(p_size, GL_FLOAT));
	}; 

	/// Size specifies number of uints required for the attribute.
	void addUintAttrib(GLuint p_size) { 
		totalVertSize += sizeof(GLuint) * p_size;
		attribList.push_back(Attrib(p_size, GL_UNSIGNED_INT));
	}; 

	/// Simply adds a vertex of type T to the end of the mesh list.
	void pushVertices(std::initializer_list<T> p_attribs) {
		verts.insert(verts.end(), p_attribs);
	};

	size_t getTotalVBOSize() { return verts.size(); }
	void genVBO() {
		glBindVertexArray(VAO);
		if (!VBOInitialized) {
			glGenBuffers(1, &VBO);
		}
		else {
			glDeleteBuffers(1, &VBO);
			glGenBuffers(1, &VBO);
		}
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * verts.size(), verts.data(), GL_STATIC_DRAW);

		GLint currentOffset = 0; // Offset that needs to be updated as arbitrary amounts of attributes are added. 

		for (unsigned int i = 0; i < attribList.size(); i++) {

			switch (attribList[i].type) {
			case GL_FLOAT:
				glVertexAttribPointer(i, attribList[i].size, GL_FLOAT, GL_FALSE, totalVertSize, (void*)currentOffset);
				currentOffset += (GLint)(attribList[i].size * sizeof(GLfloat));
				break;
			case GL_UNSIGNED_INT:
				glVertexAttribIPointer(i, attribList[i].size, GL_UNSIGNED_INT, totalVertSize, (void*)currentOffset);
				currentOffset += (GLint)(attribList[i].size * sizeof(GLuint));
			default:
				break;
			}
			glEnableVertexAttribArray(i);
		}
		VBOInitialized = true;
	};

	void remove() {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
		VBOInitialized = false;
		verts.clear();
		verts.swap(verts);
	};

	GLuint VBO = 0;
	GLuint VAO = 0;
	bool VBOInitialized = false;

};

