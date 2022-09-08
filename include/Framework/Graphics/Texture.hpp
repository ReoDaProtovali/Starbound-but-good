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
	};
	Texture(TextureID p_assignedID) {
		glID = 999;
		texID = p_assignedID;
		width = 0;
		height = 0;
		filteringMode = GL_NEAREST;
		wrappingMode = GL_REPEAT;
	};
	Texture(unsigned int p_width, unsigned int p_height, glm::vec4* p_data) {
		width = p_width;
		height = p_height;
		filteringMode = GL_NEAREST;
		wrappingMode = GL_REPEAT;
		fromVec4Data(p_width, p_height, p_data);
	}
	void setFiltering(GLint mode);
	void setWrapping(GLint mode);

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

private:
	bool initialized = false;

	GLint filteringMode;
	GLint wrappingMode;

};

#endif
