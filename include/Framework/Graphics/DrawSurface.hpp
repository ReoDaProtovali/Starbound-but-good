
#ifndef DRAWSURFACE_H
#define DRAWSURFACE_H

#include "SDL.h"
#include "GL/glew.h"
#include "util/ext/glm/vec4.hpp"
#include <vector>
#include "Framework/Graphics/DrawStates.hpp"
#include "Framework/Graphics/DrawObject.hpp"
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/GlCheck.hpp"



class DrawSurface {
public:
	DrawSurface() : viewport(glm::ivec4(0)) {};
	// Pretty static right now, but that's okay
	void insureOpenGLInit();

	// Simple function for objects that have their own draw functions. Will eventually make it to the other overload, directly below this one
	void draw(DrawObject& p_obj, DrawStates& p_states);

	// Detailed generic draw function.
	// Must be typenamed to T in order to accept any mesh vertex format, but everything that uses the template type is internal to mesh.
	template<typename T>
	void draw(Mesh<T> p_mesh, GLenum p_primativeType, DrawStates& p_states);

	void setViewport(int p_x, int p_y, int p_w, int p_h);
	void useViewport();
	void setClearColor(glm::vec4 p_col);
	void bind();

protected:
	static bool openGLInitialized;
	// If drawing to a frame buffer, it can be useful to have more than one shader output.
	std::vector<GLenum> DrawBuffers = { GL_COLOR_ATTACHMENT0 };
	// X, Y, Width, Height
	glm::ivec4 viewport;
	GLuint glBuffer = 0;
};

#endif