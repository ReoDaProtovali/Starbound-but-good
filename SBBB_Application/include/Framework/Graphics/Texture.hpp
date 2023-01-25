#pragma once

#ifndef  TEXTURE_H
#define TEXTURE_H

#include "util/utils.hpp"
#include "Framework/FrameworkConstants.hpp"
#include <GL/glew.h>


class Texture // Handles the actual GL textures, doesn't contain image data, but a GL ID
{
public:
	Texture();
	Texture(TextureID p_assignedID);
	Texture(uint32_t p_width, uint32_t p_height, glm::vec4* p_data);
	Texture(uint32_t p_width, uint32_t p_height, glm::vec4* p_data, GLenum p_type);
	// Texture copying is not super recommended
	// Texture(const Texture&) = delete;
	void setFiltering(GLint p_mode);
	void setWrapping(GLint p_mode);
	void setType(GLenum p_type);

	void fromByteData(uint32_t p_width, uint32_t p_height, unsigned char* p_data);
	void fromVec4Data(uint32_t p_width, uint32_t p_height, glm::vec4* p_data);

	/// Will delete existing texture data
	void changeDimensions(uint32_t p_width, uint32_t p_height); 

	void subVec4Data(glm::vec4* p_data);

	void remove();

	uint32_t getPixelCount() { return width * height; }
	uint32_t width = 0;
	uint32_t height = 0;

	// todo: automatic memory management.
	// not of high urgency, because textures are mainly all loaded at the start by the resource handler.
	// but would be nice for dynamic texture loading such that they don't pile up in vram.
	GLuint glID = 0;
	TextureID texID = TextureID::NO_TEXTURE;
	GLenum type = GL_TEXTURE_2D;

	bool initialized = false;

private:

	GLint m_filteringMode = GL_NEAREST;
	GLint m_wrappingMode = GL_REPEAT;

};

#endif
