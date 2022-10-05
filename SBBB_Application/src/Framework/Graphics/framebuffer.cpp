#include "Framework/Graphics/FrameBuffer.hpp"


FrameBuffer::FrameBuffer() : m_dimensions(glm::uvec2(0, 0)), m_depthBuffer(GL_NONE) {
	colorTextures.push_back(Texture());
}

void FrameBuffer::setDimensions(unsigned int p_width, unsigned int p_height)
{
	m_dimensions.x = p_width;
	m_dimensions.y = p_height;
	m_viewport = glm::ivec4(0, 0, p_width, p_height);
	if (!m_initialized) {
		init();
	}
	for (int i = 0; i < colorTextures.size(); i++) {
		colorTextures[i].changeDimensions(m_dimensions.x, m_dimensions.y);
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->ID));
		// only supports 2d color attachments for now. logical assumption.
		glCheck(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTextures[i].glID, 0));
	}


	if (m_useDepth) {
		glCheck(glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer));
		glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_dimensions.x, m_dimensions.y)); // rescale depth buffer
		glCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer));
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	useViewport();

}

void FrameBuffer::setDimensions(glm::uvec2 p_dimensions)
{
	m_dimensions = p_dimensions;
	m_viewport = glm::ivec4(0, 0, p_dimensions.x, p_dimensions.y);
	if (!m_initialized) {
		init();
	}
	for (int i = 0; i < colorTextures.size(); i++) {
		colorTextures[i].changeDimensions(m_dimensions.x, m_dimensions.y);
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->ID));
		// only supports 2d color attachments for now. logical assumption.
		glCheck(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTextures[i].glID, 0));
	}


	if (m_useDepth) {
		glCheck(glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer));
		glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_dimensions.x, m_dimensions.y)); // rescale depth buffer
		glCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer));
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	useViewport();
}

void FrameBuffer::init() {
	glCheck(glGenFramebuffers(1, &m_frameBuffer->ID));

	if (m_useDepth) {
		glCheck(glGenRenderbuffers(1, &m_depthBuffer));
	}

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->ID));

	for (int i = 0; i < colorTextures.size(); i++) {
		// Allocates texture
		colorTextures[i].changeDimensions(m_dimensions.x, m_dimensions.y);
		// constant for now
		colorTextures[i].setFiltering(GL_LINEAR);
		glCheck(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTextures[i].glID, 0));

	}

	if (m_useDepth) {
		glCheck(glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer));
		glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_dimensions.x, m_dimensions.y));
		glCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer));
	}
	glCheck(glDrawBuffers((GLsizei)m_DrawBuffers.size(), m_DrawBuffers.data()));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_initialized = true;
}

void FrameBuffer::setColorAttachments(std::initializer_list<GLenum> p_attachments)
{
	m_DrawBuffers = p_attachments;
}


GLenum FrameBuffer::getColorTexID(size_t p_index)
{
	if (p_index > colorTextures.size() - 1) throw std::exception("Tried to get frame buffer color attachment outside of range.");
	return colorTextures[p_index].glID;
}