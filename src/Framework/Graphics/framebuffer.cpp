#include "Framework/Graphics/FrameBuffer.hpp"

void FrameBuffer::setDimensions(unsigned int p_width, unsigned int p_height)
{
	dimensions.x = p_width;
	dimensions.y = p_height;
	if (!initialized) {
		init();
	}
	FBOTexture.changeDimensions(dimensions.x, dimensions.y);
	glBindFramebuffer(GL_FRAMEBUFFER, glFBO);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOTexture.glID, 0);

	if (useDepth) {
		glBindRenderbuffer(GL_RENDERBUFFER, glDepthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, dimensions.x, dimensions.y); // rescale depth buffer
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthBuffer);
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::setDimensions(glm::uvec2 p_dimensions)
{
	dimensions = p_dimensions;
	if (!initialized) {
		init();
	}
	FBOTexture.changeDimensions(dimensions.x, dimensions.y);
	glBindFramebuffer(GL_FRAMEBUFFER, glFBO);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOTexture.glID, 0);

	if (useDepth) {
		glBindRenderbuffer(GL_RENDERBUFFER, glDepthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, dimensions.x, dimensions.y); // rescale depth buffer
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthBuffer);
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::init() {
	glGenFramebuffers(1, &glFBO);

	if (useDepth) {
		glGenRenderbuffers(1, &glDepthBuffer);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, glFBO);

	FBOTexture.changeDimensions(dimensions.x, dimensions.y); // Allocates the texture
	FBOTexture.setFiltering(GL_LINEAR); // Static for now

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOTexture.glID, 0);

	if (useDepth) {
		glBindRenderbuffer(GL_RENDERBUFFER, glDepthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, dimensions.x, dimensions.y);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthBuffer);
	}
	glDrawBuffers(1, DrawBuffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	initialized = true;
}

void FrameBuffer::bindAsRenderTarget() {
	glViewport(0, 0, dimensions.x, dimensions.y);
	glBindFramebuffer(GL_FRAMEBUFFER, glFBO);
	glDrawBuffers(1, DrawBuffers);
}

GLenum FrameBuffer::getColorTexID()
{
	return FBOTexture.glID;
}
