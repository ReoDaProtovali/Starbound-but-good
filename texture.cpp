#include "Texture.hpp"

void Texture::setFiltering(GLint mode) {
	filteringMode = mode;
	if (initialized) {
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringMode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
void Texture::setWrapping(GLint mode) {
	wrappingMode = mode;
	if (initialized) {
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
void Texture::fromImage(Image& p_image)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringMode);

	glTexImage2D( // actually put the image data into the texture buffer
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		p_image.dimensions.x,
		p_image.dimensions.y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		p_image.data);
	// WARNING:: very picky about if an image in in RGB format or RBGA format. Try to keep them all RGBA with a bit depth of 8
	glBindTexture(GL_TEXTURE_2D, 0);
	initialized = true;
}

void Texture::fromVec4Data(unsigned int p_width, unsigned int p_height, glm::vec4* p_data)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringMode);

	glTexImage2D( // actually put the image data into the texture buffer
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		p_width,
		p_height,
		0,
		GL_RGBA,
		GL_FLOAT,
		p_data);
	// WARNING:: very picky about if an image in in RGB format or RBGA format. Try to keep them all RGBA with a bit depth of 8
	glBindTexture(GL_TEXTURE_2D, 0);
	initialized = true;
}

void Texture::subVec4Data(glm::vec4* p_data) {
	if (initialized) {
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, p_data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		throw std::exception("Texture not initialized, so data cannot be substituted.");
	}
}