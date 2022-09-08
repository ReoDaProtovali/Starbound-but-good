#pragma once
#include <GL/glew.h>
#include <util/ext/glm/vec2.hpp>
#include "Framework/Graphics/Texture.hpp"

// Wrapper class for a FBO, which enables rendering directly into textures instead of the screen. Very handy for post processing and offscreen rendering.
class FrameBuffer {
public:
	FrameBuffer() : dimensions(glm::uvec2(0, 0)), glFBO(GL_NONE), glDepthBuffer(GL_NONE) {};
	
	void setDimensions(unsigned int p_width, unsigned int p_height);
	void setDimensions(glm::uvec2 p_dimensions);
	void init();
	void bindAsRenderTarget();
	GLenum getColorTexID();
private:

	/// Color attatchments being drawn to by the shaders, for multiple shader outputs if required
	// Only one in here for layout(location = 0) for now. Might make it variable if I feel like it.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	
	// x width, y height.
	glm::uvec2 dimensions;
	// The internal opengl FBO ID
	GLuint glFBO;
	/// Texture that contains the color texture.
	Texture FBOTexture;
	/// The GL buffer ID for the FBO's depth buffer.
	GLuint glDepthBuffer;
	bool useDepth = true;
	bool initialized = false;

};