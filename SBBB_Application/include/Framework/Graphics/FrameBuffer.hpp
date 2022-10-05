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
// NOTE:: CURRENTLY FIXED TO TEXTURE COLOR ATTACHMENTS, AND RENDERBUFFER DEPTH.
// MIGHT MAKE A SEPARATE RENDERBUFFER-ONLY DOUBLE BUFFER LATER.
class FrameBuffer : public DrawSurface {
public:
	// Defaults to a blank color texture.
	FrameBuffer();
	// Affects the size of all color attachments
	void init();
	void setDimensions(uint32_t p_width, uint32_t p_height);
	void setDimensions(glm::uvec2 p_dimensions);
	void setColorAttachments(std::initializer_list<GLenum> p_attachments);
	GLenum getColorTexID(size_t p_index);
private:
	
	// x width, y height.
	glm::uvec2 m_dimensions;
	/// Vector that contains the color textures.
	std::vector<Texture> m_colorTextures;
	/// The GL buffer ID for the FBO's depth buffer.
	GLuint m_depthBuffer;
	bool m_useDepth = true;
	bool m_initialized = false;

};
#endif