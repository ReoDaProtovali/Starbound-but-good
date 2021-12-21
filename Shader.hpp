#pragma once

#include "GL/glew.h"
#include <glm.hpp>
#include <vector>
#include <string>

class Shader
{
	public:
		Shader() : programID(GL_NONE){};
		Shader(const char* vs_filePath, const char* fs_filePath);
		GLuint compileShaders(const char* vs_filePath, const char* fs_filePath);
		void use();
		void setBoolUniform(const std::string& p_name, bool p_value) const;
		void setIntUniform(const std::string& p_name, GLint p_value) const;
		void setFloatUniform(const std::string& p_name, GLfloat p_value) const;
		void setMat4Uniform(const std::string& p_name, glm::mat4 p_value) const;

		GLuint programID;

};

