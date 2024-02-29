#ifndef DRAWSURFACE_H
#define DRAWSURFACE_H

#include "GL/glew.h"
#include "util/ext/glm/vec4.hpp"
#include <vector>
#include "GameConstants.hpp"
#include "Framework/Graphics/DrawStates.hpp"
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/GlCheck.hpp"
#include "Framework/Graphics/GlIDs.hpp"

class DrawSurface {
public:
	DrawSurface() : m_viewport(glm::ivec4(0)) {};

	// Simple function for objects that have their own draw functions. Will eventually make it to the other overload, directly below this one
	//void draw(DrawObject& p_obj, DrawStates& p_states);

	// Detailed generic draw function.
	// Must be typenamed to T in order to accept any mesh vertex format, but everything that uses the template type is internal to mesh.
	template<typename T>
	void draw(Mesh<T>& p_mesh, GLenum p_primitiveType, DrawStates& p_states, bool p_selfBindShader = true) {
#ifdef SBBB_DEBUG
#endif
		if (!p_states.checkIfInit()) return;

		auto shader = p_states.m_shaderPtr;
		assert(shader);

		glCheck(glBindVertexArray(p_mesh.VAO->ID));
		if (p_selfBindShader)
			shader->use();

		// Bind all textures to the correct texture units
		for (size_t i = 0; i < p_states.m_textures.size(); i++) {
			glCheck(glActiveTexture(GL_TEXTURE0 + (GLenum)i));
			glCheck(glBindTexture(p_states.m_textures[i].type, p_states.m_textures[i].glID->ID));
		}

		if (p_states.m_blendMode.disabled) {
			glDisable(GL_BLEND);
		}
		else {
			glEnable(GL_BLEND);
			glCheck(glBlendFuncSeparate(p_states.m_blendMode.srcRGB, p_states.m_blendMode.dstRGB, p_states.m_blendMode.srcAlpha, p_states.m_blendMode.dstAlpha));
			glCheck(glBlendEquationSeparate(p_states.m_blendMode.RGBequation, p_states.m_blendMode.AlphaEquation));
		}

		// We'll assume that every shader uses a transform matrix, because that's pretty much a given.
		shader->setMat4UniformStatic(shader->getUniformLoc("transform"), p_states.m_transform);
#ifndef DISABLE_DEBUG_STATS
		globals.debug.drawCalls++;
#endif
		if (p_mesh.IBOInitialized) {
			glCheck(glDrawElements(p_primitiveType, (GLsizei)p_mesh.getTotalIBOSize(), GL_UNSIGNED_INT, 0));
		}
		else {
			if (p_mesh.isFeedbackMesh) {
				// these are the only three things a feedback mesh can be
				if (p_primitiveType == GL_TRIANGLES)
					glCheck(glDrawArrays(p_primitiveType, 0, (GLsizei)p_mesh.getCapturedPrimitiveCount() * 3));
				else if (p_primitiveType == GL_LINES)
					glCheck(glDrawArrays(p_primitiveType, 0, (GLsizei)p_mesh.getCapturedPrimitiveCount() * 2));
				else {
					glCheck(glDrawArrays(p_primitiveType, 0, (GLsizei)p_mesh.getCapturedPrimitiveCount()));
				}
			}
			else {
				glCheck(glDrawArrays(p_primitiveType, 0, (GLsizei)p_mesh.getTotalVBOSize()));

			}

		}

		// Causes a slight performace hitch?
		glBindVertexArray(0);
		// Unbind all textures
		// Causes an exception??? Bro??
		//for (size_t i = 0; i < p_states.textures.size(); i++) {
		//	glCheck(glActiveTexture(GL_TEXTURE0 + i));
		//	glBindTexture(p_states.textures[i].lock()->type, 0);
		//}
	};

	void setViewport(int p_x1, int p_y1, int p_x2, int p_y2) {
		m_viewport = glm::ivec4(p_x1, p_y1, p_x2, p_y2);
	};
	void useViewport() {
		glCheck(glViewport(m_viewport.x, m_viewport.y, m_viewport.z, m_viewport.w));
	};
	void setClearColor(glm::vec4 p_col) {
		glCheck(glClearColor(p_col.r, p_col.g, p_col.b, p_col.a));
	};
	void clear() {
		bind();
		glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
	// Clears the color buffer at the given attachment.
	void clearRegion(GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height, GLfloat p_col[], size_t p_bufferIndex = 0) {
		bind();
		glCheck(glEnable(GL_SCISSOR_TEST));
		glCheck(glScissor(p_x, p_y, p_width, p_height));
		glCheck(glClearBufferfv(GL_COLOR, (GLint)p_bufferIndex, p_col));
		glCheck(glDisable(GL_SCISSOR_TEST));
	}
	void bind()
	{
		useViewport();
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->ID));
		glCheck(glDrawBuffers((GLsizei)m_DrawBuffers.size(), (const GLenum*)m_DrawBuffers.data()));
	};
	glm::ivec4 getViewport() {
		return m_viewport;
	}
	float getViewportHeight() {
		return float(m_viewport.w - m_viewport.y);
	}
	float getViewportWidth() {
		return float(m_viewport.z - m_viewport.x);
	}
	float getAspect() {
		return float(m_viewport.z - m_viewport.x) / float(m_viewport.w - m_viewport.y);
	}
protected:
	// If drawing to a frame buffer, it can be useful to have more than one shader output.
	std::vector<GLenum> m_DrawBuffers = { GL_COLOR_ATTACHMENT0 };
	// X, Y, Width, Height
	glm::ivec4 m_viewport;
	std::unique_ptr<glFrameBuffer> m_frameBuffer = std::make_unique<glFrameBuffer>();
};

#endif