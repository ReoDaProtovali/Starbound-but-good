#pragma once

#include "GL/glew.h"
#include <glm.hpp>
#include <vector>
#include <string>

/// A wrapper for an OpenGL shader program, allows simple compilation and uniform setting.
class Shader
{
	public:
		/// Default void constructor. If no parameters are supplied, the program ID is set to 0, which might have adverse effects.
		Shader() : programID(GL_NONE){};
		/** Constructor that initializes by compiling the shader.
		* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
		* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
		*/
		Shader(const char* vs_filePath, const char* fs_filePath);
		/** Compiles and sets the Shader's program ID.
		* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
		* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
		* @returns The GLuint program ID given after compilation
		*/
		GLuint compileShaders(const char* vs_filePath, const char* fs_filePath);
		/// Bind the shader for use with subsequent OpenGL draw calls.
		void use();
		void setBoolUniform(const std::string& p_name, bool p_value) const;
		void setIntUniform(const std::string& p_name, GLint p_value) const;
		void setFloatUniform(const std::string& p_name, GLfloat p_value) const;
		void setTexUniform(const std::string& p_name, GLuint p_value);
		void setMat4Uniform(const std::string& p_name, glm::mat4 p_value) const;
		bool texUniformExists(GLuint p_uniformID); /// Unused
		GLuint programID; /// The OpenGL shader program ID
		std::vector<GLuint> uniforms; /// Unused

};

