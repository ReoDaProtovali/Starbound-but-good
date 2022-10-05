#include "Framework/Graphics/Texture.hpp"
#include "util/utils.hpp"

#include "Framework/Log.hpp"

Texture::Texture()
{

}
Texture::Texture(TextureID p_assignedID) :
	texID(p_assignedID)
{
}
Texture::Texture(unsigned int p_width, unsigned int p_height, glm::vec4* p_data) :
	width(p_width),
	height(p_height)
{
	fromVec4Data(width, height, p_data);
}
Texture::Texture(unsigned int p_width, unsigned int p_height, glm::vec4* p_data, GLenum p_type) :
	width(p_width),
	height(p_height),
	type(p_type)
{
	fromVec4Data(width, height, p_data);
}
//Texture::~Texture()
//{
//	remove();
//}

void Texture::setFiltering(GLint p_mode) {
	filteringMode = p_mode;
	if (initialized) {
		glBindTexture(type, glID);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filteringMode);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filteringMode);
		glBindTexture(type, 0);
	}
}
void Texture::setWrapping(GLint p_mode) {
	wrappingMode = p_mode;
	if (initialized) {
		glBindTexture(type, glID);
		glTexParameteri(type, GL_TEXTURE_WRAP_S, wrappingMode);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, wrappingMode);
		glBindTexture(type, 0);
	}
}

void Texture::setType(GLenum p_type)
{
	type = p_type;
}

void Texture::fromByteData(unsigned int p_width, unsigned int p_height, unsigned char* p_data)
{
	width = p_width;
	height = p_height;
	if (!initialized) glGenTextures(1, &glID);
	glBindTexture(type, glID); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, wrappingMode);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, wrappingMode);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filteringMode);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filteringMode);

	glTexImage2D( // actually put the image data into the texture buffer
		type,
		0,
		GL_RGBA,
		p_width,
		p_height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		p_data);
	// WARNING:: very picky about if an image in in RGB format or RBGA format. Try to keep them all RGBA with a bit depth of 8
	glBindTexture(type, 0);
	initialized = true;
}

void Texture::fromVec4Data(unsigned int p_width, unsigned int p_height, glm::vec4* p_data)
{
	if (!initialized) glGenTextures(1, &glID);
	glBindTexture(type, glID); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, wrappingMode);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, wrappingMode);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filteringMode);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filteringMode);

	glTexImage2D( // actually put the image data into the texture buffer
		type,
		0,
		GL_RGBA,
		p_width,
		p_height,
		0,
		GL_RGBA,
		GL_FLOAT,
		p_data);
	// WARNING:: very picky about if an image in in RGB format or RBGA format. Try to keep them all RGBA with a bit depth of 8
	glBindTexture(type, 0);
	initialized = true;
}

void Texture::changeDimensions(unsigned int p_width, unsigned int p_height)
{
	width = p_width;
	height = p_height;

	if (!initialized) {
		glGenTextures(1, &glID);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(type, GL_TEXTURE_WRAP_S, wrappingMode);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, wrappingMode);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filteringMode);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filteringMode);
	}
	glBindTexture(type, glID); // into the main texture buffer

	glTexImage2D( // actually put the image data into the texture buffer
		type,
		0,
		GL_RGBA,
		p_width,
		p_height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		NULL);

	// WARNING:: very picky about if an image in in RGB format or RBGA format. Try to keep them all RGBA with a bit depth of 8
	glBindTexture(type, 0);
	initialized = true; // data is allocated, so we'll just say it's initialized even if the data is undefined. Solves a bug.
}

void Texture::subVec4Data(glm::vec4* p_data) {
	CONDITIONAL_LOG(!initialized, "Texture not initialized, so data cannot be substituted.");
	if (!initialized) return;
	glBindTexture(type, glID);
	glTexSubImage2D(type, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, p_data);
	glBindTexture(type, 0);
}

void Texture::remove() {
#ifdef DELETELOGGING_ENABLED
	std::cout << "Texture removed " << glID << std::endl;
#endif
	glDeleteTextures(1, &glID);
	initialized = false;
}