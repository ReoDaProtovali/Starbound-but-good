#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <util/ext/glm/glm.hpp>
#include <vector>
#include <string>
#include "Framework/Log.hpp"
#include "Framework/Graphics/Uniform.hpp"
#include "GlIDs.hpp"

/// A wrapper for an OpenGL shader program, allows simple compilation and uniform setting.
class Shader
{
public:
	/// Default void constructor. If no parameters are supplied, the program ID is set to 0, which might have adverse effects.
	Shader() {};
	/** Constructor that initializes by compiling the shader.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	*/
	Shader(const char* vs_filePath, const char* fs_filePath);
	/** Constructor that initializes by compiling the shader, alongside a list of uniforms to set.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	* @param p_uniforms - A vector containing uniform objects, defined just above within Shader.hpp. Used to allow single-line init.
	*/
	Shader(const char* vs_filePath, const char* fs_filePath, std::vector<Uniform> p_uniforms);
	/** Constructor that initializes by compiling the shader, geometry shader included.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param gs_filePath - The filepath to the GLSL geometry shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	* @param p_uniforms - A vector containing uniform objects, defined just above within Shader.hpp. Used to allow single-line init.
	*/
	Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath, std::vector<Uniform> p_uniforms);
	/** Constructor that initializes by compiling the shader, geometry shader included.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param gs_filePath - The filepath to the GLSL geometry shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	*/
	Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath);

	Shader& operator=(const Shader& p_other);

	// Move constructor, to handle our pointer
	Shader(Shader&& other) noexcept;
	/** Compiles and sets the Shader's program ID.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	* @returns The GLuint program ID given after compilation
	*/
	GLuint compileShaders(const char* vs_filePath, const char* fs_filePath);
	/** Compiles and sets the Shader's program ID. Overload for incorporating geometry shader.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param gs_filePath - The filepath to the GLSL geometry shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	* @returns The GLuint program ID given after compilation
	*/
	GLuint compileShaders(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath);

	/// Bind the shader for use with subsequent OpenGL draw calls.
	void use() const;
	/// A function that handles assigning a value to a uniform within the shader's program. In this case, a boolean.
	void setBoolUniform(const std::string& p_name, bool p_value) const;
	void setBoolUniform(GLint p_loc, bool p_value) const;
	/// A function that handles assigning a value to a uniform within the shader's program. In this case, an integer.
	void setIntUniform(const std::string& p_name, GLint p_value) const;
	void setIntUniform(GLint p_loc, GLint p_value) const;
	/// A function that handles assigning a value to a uniform within the shader's program. In this case, a float.
	void setFloatUniform(const std::string& p_name, GLfloat p_value) const;
	void setFloatUniform(GLint p_loc, GLfloat p_value) const;
	/// A function that handles assigning a value to a uniform within the shader's program. In this case, an OpenGL texture.
	void setTexUniform(const std::string& p_name, GLuint p_value);
	void setTexUniform(GLint p_loc, GLuint p_value);
	/// A function that handles assigning a value to a uniform within the shader's program. In this case, a 4x4 matrix.
	void setMat4Uniform(const std::string& p_name, glm::mat4 p_value) const;
	void setMat4Uniform(GLint p_loc, glm::mat4 p_value) const;

	void setVec2Uniform(const std::string& p_name, glm::vec2 p_value) const;
	void setVec2Uniform(GLint p_loc, glm::vec2 p_value) const;
	// Used to set a list of uniforms using uniform objects.
	// Only works with strings, no locations.
	void setUniforms(std::vector<Uniform> p_uniforms);

	std::shared_ptr<glProgram> program = std::make_shared<glProgram>();
};

#endif