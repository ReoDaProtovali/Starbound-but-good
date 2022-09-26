#pragma once

#ifndef  TEXTURE_H
#define TEXTURE_H

#include "util/utils.hpp"
#include "GameConstants.hpp"
#include <GL/glew.h>


class Texture // Handles the actual GL textures, doesn't contain image data, but a GL ID
{
public:
	Texture();
	Texture(TextureID p_assignedID);
	Texture(unsigned int p_width, unsigned int p_height, glm::vec4* p_data);
	Texture(unsigned int p_width, unsigned int p_height, glm::vec4* p_data, GLenum p_type);
	// Texture copying is not super recommended
	// Texture(const Texture&) = delete;
	void setFiltering(GLint p_mode);
	void setWrapping(GLint p_mode);
	void setType(GLenum p_type);

	void fromByteData(unsigned int p_width, unsigned int p_height, unsigned char* p_data);
	void fromVec4Data(unsigned int p_width, unsigned int p_height, glm::vec4* p_data);

	/// Will delete existing texture data
	void changeDimensions(unsigned int p_width, unsigned int p_height); 

	void subVec4Data(glm::vec4* p_data);

	void remove();

	unsigned int getPixelCount() { return width * height; }
	unsigned int width = 0;
	unsigned int height = 0;

	GLuint glID = 0;
	TextureID texID = TextureID::NO_TEXTURE;
	GLenum type = GL_TEXTURE_2D;

private:
	bool initialized = false;
	// Used for managing when the OpenGL texture gets deleted, much like a shared_ptr
	int refCount = 0;

	GLint filteringMode = GL_NEAREST;
	GLint wrappingMode = GL_REPEAT;

};

#endif
