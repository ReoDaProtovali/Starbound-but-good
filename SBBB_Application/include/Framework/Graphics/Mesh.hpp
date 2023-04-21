#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include <vector>
#include <initializer_list>
#include "Framework/Log.hpp"
#include "GlCheck.hpp"
#include "GlIDs.hpp"
#include <assert.h>


enum {
	NO_VAO_INIT = false,
	VAO_INIT = true
};

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
	// THESE MEAN THAT THE OPENGL OBJECT HAS BEEN CREATED. IT DOES NOT MEAN IT NEEDS TO HAVE DATA.
	bool VAOInitialized = false;
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
		VAOInitialized = true;
	}

	Mesh(bool initVAO)
	{
		if (!initVAO) return;
		glGenVertexArrays(1, &VAO->ID);
		GLGEN_LOG("Generated Vertex Array " << VAO->ID);
		VAOInitialized = true;
	}

	// Mesh copying is a pretty heavy operation, so try not to
	Mesh(const Mesh& p_other) :
		m_verts(p_other.m_verts),
		m_indices(p_other.m_indices),
		m_attribList(p_other.m_attribList),
		m_singleVertexSize(p_other.m_singleVertexSize),
		m_GPUVertsSize(p_other.m_GPUVertsSize),
		m_streamType(p_other.m_streamType)
	{
		// We can't copy these, so we have to create new opengl buffers. Copying is discouraged.
		GLGEN_LOG("Copied mesh! VAO: " << p_other.VAO->ID);
		VBO = std::make_unique<glBuffer>();
		VAO = std::make_unique<glVertexArray>();
		IBO = std::make_unique<glBuffer>();
		glGenVertexArrays(1, &VAO->ID);
		GLGEN_LOG("Generated Vertex Array " << VAO->ID);

		//pushVBOToGPU();
		//pushIBOToGPU();
	}

	Mesh<T>& operator=(const Mesh<T>& p_other) {
		m_verts = p_other.m_verts;
		m_indices = p_other.m_indices;
		m_attribList = p_other.m_attribList;
		m_singleVertexSize = p_other.m_singleVertexSize;
		m_GPUVertsSize = p_other.m_GPUVertsSize;
		m_streamType = p_other.m_streamType;

		// We can't copy these, so we have to create new opengl buffers. Copying is discouraged.
		GLGEN_LOG("Copied mesh! VAO: " << p_other.VAO->ID);
		VBO = std::make_unique<glBuffer>();
		VAO = std::make_unique<glVertexArray>();
		IBO = std::make_unique<glBuffer>();
		glGenVertexArrays(1, &VAO->ID);
		GLGEN_LOG("Generated Vertex Array " << VAO->ID);

		//pushVBOToGPU();
		//pushIBOToGPU();
		return *this;
	}

	Mesh<T>& operator=(Mesh<T>&& p_other) {
		m_verts = p_other.m_verts;
		m_indices = p_other.m_indices;
		m_attribList = p_other.m_attribList;
		m_singleVertexSize = p_other.m_singleVertexSize;
		m_GPUVertsSize = p_other.m_GPUVertsSize;
		m_streamType = p_other.m_streamType;

		GLGEN_LOG("Moved mesh! VAO: " << p_other.VAO->ID);

		VBO = std::move(p_other.VBO);
		VAO = std::move(p_other.VAO);
		IBO = std::move(p_other.IBO);

		return *this;
	}

	// Move constructor for rvalues
	Mesh(Mesh&& p_other) noexcept :
		m_verts(p_other.m_verts),
		m_indices(p_other.m_indices),
		m_attribList(p_other.m_attribList),
		m_singleVertexSize(p_other.m_singleVertexSize),
		m_GPUVertsSize(p_other.m_GPUVertsSize),
		m_streamType(p_other.m_streamType),
		VBOInitialized(p_other.VBOInitialized),
		IBOInitialized(p_other.IBOInitialized)
	{
		GLGEN_LOG("Moved mesh! VAO: " << p_other.VAO->ID);

		VBO = std::move(p_other.VBO);
		VAO = std::move(p_other.VAO);
		IBO = std::move(p_other.IBO);

	}
	/// Allows you to skip a few memory allocations if you reserve it right off the bat.
	void reserve(size_t p_reserveAmount) { m_verts.reserve(p_reserveAmount); }

	void setStreamType(GLenum p_type) {
#ifdef SBBB_DEBUG
		if (!(p_type == GL_STREAM_DRAW || p_type == GL_DYNAMIC_DRAW || p_type == GL_STATIC_DRAW)) {
			throw std::runtime_error("Used an invalid enum to set a stream type.");
			return;
		}
#endif
		m_streamType = p_type;
	}

	/// Size specifies number of floats required for the attribute.
	void addFloatAttrib(GLuint p_size) {
		m_singleVertexSize += sizeof(GLfloat) * p_size;
		m_attribList.emplace_back(p_size, GL_FLOAT);
	}; 

	/// Size specifies number of uints required for the attribute.
	void addUintAttrib(GLuint p_size) { 
		m_singleVertexSize += sizeof(GLuint) * p_size;
		m_attribList.emplace_back(p_size, GL_UNSIGNED_INT);
	}; 

	/// Size specifies number of bytes required for the attribute.
	void addUbyteAttrib(GLubyte p_size) {
		// I don't think things can be smaller than 4 bytes, so the size of a ubyte is actually 4 on the gpu
		m_singleVertexSize += sizeof(GLuint) * p_size;
		m_attribList.emplace_back(p_size, GL_UNSIGNED_BYTE);
	};

	void addIntAttrib(GLubyte p_size) {
		m_singleVertexSize += sizeof(GLint) * p_size;
		m_attribList.emplace_back(p_size, GL_INT);
	};

	void pushVertex(T& vert) {
		m_verts.push_back(vert);
		m_GPUVertsSize += 1;
	}
	/// Simply adds a vertex of type T to the end of the mesh list.
	void pushVertices(const std::initializer_list<T>& p_attribs) {
		m_verts.insert(m_verts.end(), p_attribs);
		m_GPUVertsSize += (uint32_t)p_attribs.size();
	};

	void pushIndices(const std::initializer_list<GLuint>& p_attribs) {
		m_indices.insert(m_indices.end(), p_attribs);
		m_GPUIndicesSize += (uint32_t)p_attribs.size();
	}

	bool hasData() { return (bool)m_verts.size(); };
	// confusing, but it's the count of attributes that were sent to the gpu at some point.
	size_t getTotalVBOSize() { return m_GPUVertsSize; }
	size_t getTotalIBOSize() { return m_GPUIndicesSize; }

	size_t getStoredVertCount() { return m_verts.size(); }

	GLuint* getIBOPointer() { return m_indices.data(); }


	void pushVBOToGPU() {
		if (!VAOInitialized) {
			glGenVertexArrays(1, &VAO->ID);
			GLGEN_LOG("Generated Vertex Array " << VAO->ID);
			VAOInitialized = true;
		}
		glBindVertexArray(VAO->ID);
		if (!VBOInitialized) {
			glGenBuffers(1, &VBO->ID);
			GLGEN_LOG("Generated Vertex Buffer " << VBO->ID);
			VBOInitialized = true;
		}
		else {
			GLGEN_LOG("Re-generated Vertex Buffer " << VBO->ID);
		}
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, VBO->ID));
		glCheck(glBufferData(GL_ARRAY_BUFFER, sizeof(T) * m_verts.size(), m_verts.data(), m_streamType));

		uint64_t currentOffset = 0; // Offset that needs to be updated as arbitrary amounts of attributes are added. 

		for (uint32_t i = 0; i < m_attribList.size(); i++) {

			switch (m_attribList[i].type) { // Will add more attribute types as I need them. 
			case GL_FLOAT:
				glCheck(glVertexAttribPointer(i, m_attribList[i].size, GL_FLOAT, GL_FALSE, m_singleVertexSize, reinterpret_cast<const void*>(currentOffset)));
				currentOffset += (GLint)(m_attribList[i].size * sizeof(GLfloat));
				break;
			case GL_UNSIGNED_INT:
				glCheck(glVertexAttribIPointer(i, m_attribList[i].size, GL_UNSIGNED_INT, m_singleVertexSize, reinterpret_cast<const void*>(currentOffset)));
				currentOffset += (GLint)(m_attribList[i].size * sizeof(GLuint));
				break;
			case GL_INT:
				glCheck(glVertexAttribIPointer(i, m_attribList[i].size, GL_INT, m_singleVertexSize, reinterpret_cast<const void*>(currentOffset)));
				currentOffset += (GLint)(m_attribList[i].size * sizeof(GLuint));
				break;
			case GL_UNSIGNED_BYTE:
				glCheck(glVertexAttribIPointer(i, m_attribList[i].size, GL_UNSIGNED_BYTE, m_singleVertexSize, reinterpret_cast<const void*>(currentOffset)));
				currentOffset += (GLint)(m_attribList[i].size * sizeof(GLuint));
				break;
			default:
				assert("unreachable" && 0);
				break;
			}
			glCheck(glEnableVertexAttribArray(i));
		}
		glEnableVertexAttribArray(0);
	};

	void pushIBOToGPU() {
		if (!VAOInitialized) {
			glGenVertexArrays(1, &VAO->ID);
			GLGEN_LOG("Generated Vertex Array " << VAO->ID);
			VAOInitialized = true;
		}
		if (m_indices.size() == 0) return;
		glCheck(glBindVertexArray(VAO->ID));
		if (!IBOInitialized) {
			glGenBuffers(1, &IBO->ID);
			GLGEN_LOG("Generated Index Buffer " << IBO->ID);
			IBOInitialized = true;
		}
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO->ID));
		glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indices.size(), m_indices.data(), m_streamType));
		glEnableVertexAttribArray(GL_NONE);
	}
	void subCurrentVBOData() {
		glCheck(glBindVertexArray(VAO->ID));
		if (!VBOInitialized) {
			glGenBuffers(1, &VBO->ID);
			GLGEN_LOG("Generated Vertex Buffer " << VBO->ID);
			VBOInitialized = true;
		}
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, VBO->ID));
		glCheck(glBufferSubData(GL_ARRAY_BUFFER, 0, m_verts.size() * sizeof(T), m_verts.data()));
	}
	void subVBOData(GLuint p_startIndex, GLuint p_endIndex, T* p_data) {
		glCheck(glBindVertexArray(VAO->ID));
		if (!VBOInitialized) {
			glGenBuffers(1, &VBO->ID);
			GLGEN_LOG("Generated Vertex Buffer " << VBO->ID);
			VBOInitialized = true;
		}
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, VBO->ID));
		glCheck(glBufferSubData(GL_ARRAY_BUFFER, p_startIndex * sizeof(T), (p_endIndex - p_startIndex) * sizeof(T), p_data));
	}
	// dealloc the verts
	void clean() {
		m_verts.clear();
		m_verts.shrink_to_fit();
		m_indices.clear();
		m_indices.shrink_to_fit();
	}
	void remove() {
		m_GPUVertsSize = 0;
		m_GPUIndicesSize = 0;
		m_verts = std::vector<T>();
		m_indices = std::vector<GLuint>();
	};

	private:
		std::vector<T> m_verts; // Vert data in any container you deem fit.
		std::vector<GLuint> m_indices;

		std::vector<Attrib> m_attribList; // List of attributes the shader uses.
		uint32_t m_singleVertexSize = 0;
		// used so I don't have to keep the verts around on system memory
		uint32_t m_GPUVertsSize = 0;
		uint32_t m_GPUIndicesSize = 0;
	
		// By default, vbo data is expected not to change. Be sure to set it to dynamic if that is not the case.
		GLenum m_streamType = GL_STATIC_DRAW;
};

#endif