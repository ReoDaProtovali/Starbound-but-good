#include "../include/Framework/Graphics/DrawSurface.hpp"
#include <iostream>

bool DrawSurface::openGLInitialized = false;

void DrawSurface::insureOpenGLInit()
{
	if (openGLInitialized) return;
	std::cout << "Done" << std::endl;
#ifdef LOADLOGGING_ENABLED
	std::cout << "Initializing OpenGL 3.2..." << std::endl;
#endif
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); // Bit depth of 8 bits
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // OpenGL 3.2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


	glewExperimental = GL_TRUE;
	auto init_res = glewInit();
	if (init_res != GLEW_OK)
	{
		std::cout << glewGetErrorString(glewInit()) << std::endl;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	openGLInitialized = true;

}

void DrawSurface::draw(DrawObject& p_obj, DrawStates& p_states)
{
	p_obj.draw(*this, p_states);
}

template<typename T>
void DrawSurface::draw(Mesh<T> p_mesh, GLenum p_primitiveType, DrawStates& p_states) {
	if (!p_states.checkIfInitialized()) return;
	useViewport();

	glCheck(glBindVertexArray(p_mesh.VAO));
	p_states.shader->use();

	// Bind all textures to the correct texture units
	for (size_t i = 0; i < p_states.textures.size(); i++) {
		glCheck(glActiveTexture(p_states.textures[i]->glID));
		glCheck(glBindTexture(p_states.textures[i]->type, p_states.textures[i]->glID));
	}

	if (p_states.blendMode.disabled) {
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
		glCheck(glBlendFuncSeparate(p_states.blendMode.srcRGB, p_states.blendMode.dstRGB, p_states.blendMode.srcAlpha, p_states.blendMode.dstAlpha));
		glCheck(glBlendEquationSeparate(p_states.blendMode.RGBequation, p_states.blendMode.AlphaEquation));
	}

	// We'll assume that every shader uses a transform matrix, because that's pretty much a given.
	p_states.shader->setMat4Uniform("transform", *p_states.transform);

	glDrawArrays(p_primitiveType, 0, p_mesh.getTotalVBOSize());

	glBindVertexArray(0);
	// Unbind all textures
	for (size_t i = 0; i < p_states.textures.size(); i++) {
		glCheck(glActiveTexture(p_states.textures[i]->glID));
		glBindTexture(p_states.textures[i]->type, 0);
	}
};

void DrawSurface::setViewport(int p_x, int p_y, int p_w, int p_h)
{
	viewport = glm::ivec4(p_x, p_y, p_w, p_h);
}
void DrawSurface::useViewport() {
	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
}
void DrawSurface::setClearColor(glm::vec4 p_col) {
	glClearColor(p_col.r, p_col.g, p_col.b, p_col.a);
}

void DrawSurface::bind()
{
	useViewport();
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, glBuffer));
	glCheck(glDrawBuffers(DrawBuffers.size(), DrawBuffers.data()));
}
