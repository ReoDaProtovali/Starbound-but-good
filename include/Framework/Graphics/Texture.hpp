#pragma once

#ifndef  TEXTURE_H
#define TEXTURE_H

#include "util/utils.hpp"
#include "GameConstants.hpp"
#include <GL/glew.h>

class Texture // Handles the actual GL textures, doesn't contain image data, but a GL ID
{
public:
	Texture() {
		glID = 999;
		texID = TextureID::NO_TEXTURE;
		width = 0;
		height = 0;
		filteringMode = GL_NEAREST;
		wrappingMode = GL_REPEAT;
		type = GL_TEXTURE_2D;
	};
	Texture(TextureID p_assignedID) {
		glID = 999;
		texID = p_assignedID;
		width = 0;
		height = 0;
		filteringMode = GL_NEAREST;
		wrappingMode = GL_REPEAT;
		type = GL_TEXTURE_2D;
	};
	Texture(unsigned int p_width, unsigned int p_height, glm::vec4* p_data) {
		width = p_width;
		height = p_height;
		filteringMode = GL_NEAREST;
		wrappingMode = GL_REPEAT;
		type = GL_TEXTURE_2D;
		fromVec4Data(p_width, p_height, p_data);
	}
	Texture(unsigned int p_width, unsigned int p_height, glm::vec4* p_data, GLenum p_type) {
		width = p_width;
		height = p_height;
		filteringMode = GL_NEAREST;
		wrappingMode = GL_REPEAT;
		type = p_type;
		fromVec4Data(p_width, p_height, p_data);
	}
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
	unsigned int width;
	unsigned int height;

	GLuint glID;
	TextureID texID;
	GLenum type;

private:
	bool initialized = false;

	GLint filteringMode;
	GLint wrappingMode;

};

#endif