#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <GL/glew.h>
#include <util/ext/glm/vec2.hpp>
#include "Framework/Graphics/Texture.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/GlCheck.hpp"
#include <initializer_list>
#include <vector>

// Wrapper class for a FBO, which enables rendering directly into textures instead of the screen. Very handy for post processing and offscreen rendering.
class FrameBuffer : public DrawSurface {
public:
	FrameBuffer();
	// Affects the size of all color attachments
	void setDimensions(unsigned int p_width, unsigned int p_height);
	void setDimensions(glm::uvec2 p_dimensions);
	void init();
	void setColorAttachments(std::initializer_list<GLenum> p_attachments);
	GLenum getColorTexID(size_t p_index);
private:
	
	// x width, y height.
	glm::uvec2 dimensions;
	/// Vector that contains the color textures.
	std::vector<Texture> colorTextures;
	/// The GL buffer ID for the FBO's depth buffer.
	GLuint glDepthBuffer;
	bool useDepth = true;
	bool initialized = false;

};
#endif