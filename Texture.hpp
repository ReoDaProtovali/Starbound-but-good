#pragma once

#ifndef  TEXTURE_H
#define TEXTURE_H

#include "Image.hpp"

class Texture // Handles the actual GL textures, doesn't contain image data, but a GL ID
{
public:
	Texture() {
		ID = 999;
		width = 0;
		height = 0;
		filteringMode = GL_NEAREST;
		wrappingMode = GL_REPEAT;
	};
	Texture(Image& p_image) {
		width = p_image.dimensions.x;
		height = p_image.dimensions.y;
		filteringMode = GL_NEAREST;
		wrappingMode = GL_REPEAT;
		fromImage(p_image);
	}
	Texture(unsigned int p_width, unsigned int p_height, glm::vec4* p_data) {
		width = p_width;
		height = p_height;
		filteringMode = GL_NEAREST;
		wrappingMode = GL_REPEAT;
		fromVec4Data(p_width, p_height, p_data);
	}
	void setFiltering(GLint mode);
	void setWrapping(GLint mode);

	void fromImage(Image& p_image);
	void fromVec4Data(unsigned int p_width, unsigned int p_height, glm::vec4* p_data);

	void changeDimensions(unsigned int p_width, unsigned int p_height); // Will delete existing texture data

	void subVec4Data(glm::vec4* p_data);

	unsigned int width;
	unsigned int height;

	GLuint ID;

private:
	bool initialized = false;

	GLint filteringMode;
	GLint wrappingMode;

};

#endif
