#include "Framework/Graphics/FrameBuffer.hpp"


FrameBuffer::FrameBuffer() : dimensions(glm::uvec2(0, 0)), glDepthBuffer(GL_NONE) {
	colorTextures.push_back(Texture());
	std::cout << colorTextures.size();
}

void FrameBuffer::setDimensions(unsigned int p_width, unsigned int p_height)
{
	dimensions.x = p_width;
	dimensions.y = p_height;
	viewport = glm::ivec4(0, 0, p_width, p_height);
	if (!initialized) {
		init();
	}
	for (int i = 0; i < colorTextures.size(); i++) {
		colorTextures[i].changeDimensions(dimensions.x, dimensions.y);
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, glBuffer));
		// only supports 2d color attachments for now. logical assumption.
		glCheck(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTextures[i].glID, 0));
	}


	if (useDepth) {
		glCheck(glBindRenderbuffer(GL_RENDERBUFFER, glDepthBuffer));
		glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, dimensions.x, dimensions.y)); // rescale depth buffer
		glCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthBuffer));
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	useViewport();

}

void FrameBuffer::setDimensions(glm::uvec2 p_dimensions)
{
	dimensions = p_dimensions;
	viewport = glm::ivec4(0, 0, p_dimensions.x, p_dimensions.y);
	if (!initialized) {
		init();
	}
	for (int i = 0; i < colorTextures.size(); i++) {
		colorTextures[i].changeDimensions(dimensions.x, dimensions.y);
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, glBuffer));
		// only supports 2d color attachments for now. logical assumption.
		glCheck(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTextures[i].glID, 0));
	}


	if (useDepth) {
		glCheck(glBindRenderbuffer(GL_RENDERBUFFER, glDepthBuffer));
		glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, dimensions.x, dimensions.y)); // rescale depth buffer
		glCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthBuffer));
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	useViewport();
}

void FrameBuffer::init() {
	glCheck(glGenFramebuffers(1, &glBuffer));

	if (useDepth) {
		glCheck(glGenRenderbuffers(1, &glDepthBuffer));
	}

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, glBuffer));

	for (int i = 0; i < colorTextures.size(); i++) {
		// Allocates texture
		colorTextures[i].changeDimensions(dimensions.x, dimensions.y);
		// constant for now
		colorTextures[i].setFiltering(GL_LINEAR);
		glCheck(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTextures[i].glID, 0));

	}

	if (useDepth) {
		glCheck(glBindRenderbuffer(GL_RENDERBUFFER, glDepthBuffer));
		glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, dimensions.x, dimensions.y));
		glCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthBuffer));
	}
	glCheck(glDrawBuffers((GLsizei)DrawBuffers.size(), DrawBuffers.data()));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	initialized = true;
}

void FrameBuffer::setColorAttachments(std::initializer_list<GLenum> p_attachments)
{
	DrawBuffers = p_attachments;
}


GLenum FrameBuffer::getColorTexID(size_t p_index)
{
	if (p_index > colorTextures.size() - 1) throw std::exception("Tried to get frame buffer color attachment outside of range.");
	return colorTextures[p_index].glID;
}
