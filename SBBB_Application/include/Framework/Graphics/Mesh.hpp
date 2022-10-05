#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include <vector>
#include <initializer_list>
#include "Framework/Log.hpp"
#include "GlCheck.hpp"
#include "GlIDs.hpp"


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
public:

	bool VBOInitialized = false;
	bool IBOInitialized = false;

	// Manage our opengl memory automatically
	std::unique_ptr<glBuffer> VBO = std::make_unique<glBuffer>();
	std::unique_ptr<glVertexArray> VAO = std::make_unique<glVertexArray>();
	std::unique_ptr<glBuffer> IBO = std::make_unique<glBuffer>();

	Mesh()
	{
		glGenVertexArrays(1, &VAO->ID);
		GLGEN_LOG("Generated Vertex Array " << VAO->ID);
	}

	// Mesh copying is a pretty heavy operation, so try not to
	Mesh(const Mesh& p_other) :
		verts(p_other.verts),
		indices(p_other.indices),
		attribList(p_other.attribList),
		totalVertSize(p_other.totalVertSize),
		streamType(p_other.streamType)
	{
		// We can't copy these, so we have to create new opengl buffers. Copying is discouraged.
		LOG("Copied mesh! VAO: " << p_other.VAO->ID);
		VBO = std::make_unique<glBuffer>();
		VAO = std::make_unique<glVertexArray>();
		IBO = std::make_unique<glBuffer>();
		glGenVertexArrays(1, &VAO->ID);
		GLGEN_LOG("Generated Vertex Array " << VAO->ID);

		genVBO();
		genIBO();
	}

	// Move constructor for rvalues
	Mesh(Mesh&& p_other) noexcept :
		verts(p_other.verts),
		indices(p_other.indices),
		attribList(p_other.attribList),
		totalVertSize(p_other.totalVertSize),
		streamType(p_other.streamType),
		VBOInitialized(p_other.VBOInitialized),
		IBOInitialized(p_other.IBOInitialized)
	{
		LOG("Moved mesh! VAO: " << p_other.VAO->ID);

		VBO = std::move(p_other.VBO);
		VAO = std::move(p_other.VAO);
		IBO = std::move(p_other.IBO);

	}
	/// Allows you to skip a few memory allocations if you reserve it right off the bat.
	void reserve(size_t p_reserveAmount) { verts.reserve(p_reserveAmount); }

	void setStreamType(GLenum p_type) {
#ifdef SBBB_DEBUG
		if (p_type != GL_STREAM_DRAW || p_type != GL_DYNAMIC_DRAW || p_type != GL_STATIC_DRAW) {
			throw std::runtime_error("Used an invalid enum to set a stream type.");
			return;
		}
#endif
		streamType = p_type;
	}

	/// Size specifies number of floats required for the attribute.
	void addFloatAttrib(GLuint p_size) {
		totalVertSize += sizeof(GLfloat) * p_size;
		attribList.emplace_back(p_size, GL_FLOAT);
	}; 

	/// Size specifies number of uints required for the attribute.
	void addUintAttrib(GLuint p_size) { 
		totalVertSize += sizeof(GLuint) * p_size;
		attribList.emplace_back(p_size, GL_UNSIGNED_INT);
	}; 

	/// Size specifies number of bytes required for the attribute.
	void addUbyteAttrib(GLubyte p_size) {
		totalVertSize += sizeof(GLubyte) * p_size;
		attribList.emplace_back(p_size, GL_UNSIGNED_BYTE);
	};

	/// Simply adds a vertex of type T to the end of the mesh list.
	void pushVertices(const std::initializer_list<T>& p_attribs) {
		verts.insert(verts.end(), p_attribs);
	};

	void pushIndices(const std::initializer_list<GLuint>& p_attribs) {
		indices.insert(verts.end(), p_attribs);
	}

	size_t getTotalVBOSize() { return verts.size(); }

	void genVBO() {
		glBindVertexArray(VAO->ID);
		if (!VBOInitialized) {
			glGenBuffers(1, &VBO->ID);
			GLGEN_LOG("Generated Vertex Buffer " << VBO->ID);
		}
		else {
			GLGEN_LOG("Re-generated Vertex Buffer " << VBO->ID);
		}
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, VBO->ID));
		glCheck(glBufferData(GL_ARRAY_BUFFER, sizeof(T) * verts.size(), verts.data(), GL_STATIC_DRAW));

		GLint currentOffset = 0; // Offset that needs to be updated as arbitrary amounts of attributes are added. 

		for (unsigned int i = 0; i < attribList.size(); i++) {

			switch (attribList[i].type) { // Will add more attribute types as I need them.
			case GL_FLOAT: 
				glCheck(glVertexAttribPointer(i, attribList[i].size, GL_FLOAT, GL_FALSE, totalVertSize, (const void*)currentOffset));
				currentOffset += (GLint)(attribList[i].size * sizeof(GLfloat));
				break;
			case GL_UNSIGNED_INT:
				glCheck(glVertexAttribIPointer(i, attribList[i].size, GL_UNSIGNED_INT, totalVertSize, (const void*)currentOffset));
				currentOffset += (GLint)(attribList[i].size * sizeof(GLuint));
				break;
			case GL_UNSIGNED_BYTE:
				glCheck(glVertexAttribIPointer(i, attribList[i].size, GL_UNSIGNED_BYTE, totalVertSize, (const void*)currentOffset));
				currentOffset += (GLint)(attribList[i].size * sizeof(GLubyte));
				break;
			default:
				break;
			}
			glCheck(glEnableVertexAttribArray(i));
		}
		glEnableVertexAttribArray(0);
		VBOInitialized = true;
	};

	void genIBO() {
		if (indices.size() == 0) return;
		glCheck(glBindVertexArray(VAO->ID));
		if (!IBOInitialized) {
			glGenBuffers(1, &IBO->ID);
			GLGEN_LOG("Generated Index Buffer " << IBO->ID);
		}
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO->ID));
		glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(T) * indices.size(), indices.data(), streamType));
		glEnableVertexAttribArray(0);
	}

	void subVBOData(GLuint p_startIndex, GLuint p_endIndex, T* p_data) {
		glCheck(glBindVertexArray(VAO->ID));
		if (!VBOInitialized) {
			glGenBuffers(1, &VBO->ID);
			GLGEN_LOG("Generated Vertex Buffer " << VBO->ID);
		}
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, VBO->ID));
		glCheck(glBufferSubData(GL_ARRAY_BUFFER, p_startIndex * sizeof(T), (p_endIndex - p_startIndex) * sizeof(T), p_data));
	}

	void remove() {
		VBOInitialized = false;
		IBOInitialized = false;
		verts.clear();
		verts.shrink_to_fit();
		indices.clear();
		indices.shrink_to_fit();
	};

	private:
		std::vector<T> verts; // Vert data in any container you deem fit.
		std::vector<GLuint> indices;

		std::vector<Attrib> attribList; // List of attributes the shader uses.
		unsigned int totalVertSize = 0;

		GLenum streamType = GL_STATIC_DRAW;
};

#endif