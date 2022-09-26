#ifndef DRAWSURFACE_H
#define DRAWSURFACE_H

#include "GL/glew.h"
#include "util/ext/glm/vec4.hpp"
#include <vector>
#include "Framework/Graphics/DrawStates.hpp"
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/GlCheck.hpp"

class DrawSurface {
public:
	DrawSurface() : viewport(glm::ivec4(0)) {};

	// Simple function for objects that have their own draw functions. Will eventually make it to the other overload, directly below this one
	//void draw(DrawObject& p_obj, DrawStates& p_states);

	// Detailed generic draw function.
	// Must be typenamed to T in order to accept any mesh vertex format, but everything that uses the template type is internal to mesh.
	template<typename T>
	void draw(Mesh<T>& p_mesh, GLenum p_primitiveType, DrawStates& p_states) {
		if (!p_states.checkIfInitialized()) return;
		auto shader = p_states.shader.lock();
		if (!shader) {
			std::cout << "Tried to draw with a null shader! \n";
			return;
		}

		glCheck(glBindVertexArray(p_mesh.VAO));
		shader->use();


		// Bind all textures to the correct texture units
		for (size_t i = 0; i < p_states.textures.size(); i++) {
			if (!p_states.textures[i].lock()) {
				std::cout << "Tried to draw with a null texture! \n";
			};
			glCheck(glActiveTexture(GL_TEXTURE0 + i));
			glCheck(glBindTexture(p_states.textures[i].lock()->type, p_states.textures[i].lock()->glID));
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
		shader->setMat4Uniform("transform", p_states.transform);

		glDrawArrays(p_primitiveType, 0, p_mesh.getTotalVBOSize());

		glBindVertexArray(0);
		// Unbind all textures
		// Causes an exception??? Bro??
		//for (size_t i = 0; i < p_states.textures.size(); i++) {
		//	glCheck(glActiveTexture(GL_TEXTURE0 + i));
		//	glBindTexture(p_states.textures[i].lock()->type, 0);
		//}
	};

	void setViewport(int p_x1, int p_y1, int p_x2, int p_y2) {
		viewport = glm::ivec4(p_x1, p_y1, p_x2, p_y2);
	};
	void useViewport() {
		//std::cout << viewport.z << " " << viewport.w << std::endl;
		glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
	};
	void setClearColor(glm::vec4 p_col) {
		glClearColor(p_col.r, p_col.g, p_col.b, p_col.a);
	};
	void clear() {
		bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void bind()
	{
		useViewport();
		glBindFramebuffer(GL_FRAMEBUFFER, glBuffer);
		glDrawBuffers(DrawBuffers.size(), (const GLenum*)DrawBuffers.data());
	};

protected:
	// If drawing to a frame buffer, it can be useful to have more than one shader output.
	std::vector<GLenum> DrawBuffers = { GL_COLOR_ATTACHMENT0 };
	// X, Y, Width, Height
	glm::ivec4 viewport;
	GLuint glBuffer = 0;
};

#endif