#include "AtlasBuilder.hpp"

void AtlasBuilder::initFBO()
{
	glGenFramebuffers(1, &atlasFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

	screenColorTex.changeDimensions(windowWidth, windowHeight); // Allocates the texture
	screenColorTex.setFiltering(GL_LINEAR);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenColorTex.glID, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	glDrawBuffers(1, DrawBuffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}