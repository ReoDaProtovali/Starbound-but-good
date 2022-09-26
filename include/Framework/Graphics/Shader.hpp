#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <util/ext/glm/glm.hpp>
#include <vector>
#include <string>
#include "Uniform.hpp"

/// A wrapper for an OpenGL shader program, allows simple compilation and uniform setting.
class Shader
{
public:
	/// Default void constructor. If no parameters are supplied, the program ID is set to 0, which might have adverse effects.
	Shader() : programID(GL_NONE) {};
	/** Constructor that initializes by compiling the shader.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	*/
	Shader(const char* vs_filePath, const char* fs_filePath);
	/** Constructor that initializes by compiling the shader, alongside a list of uniforms to set.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	*/
	Shader(const char* vs_filePath, const char* fs_filePath, std::vector<Uniform> p_uniforms);
	/** Constructor that initializes by compiling the shader, geometry shader included.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	* @param p_uniforms - A vector containing uniform objects, defined just above within Shader.hpp. Used to allow single-line init.
	*/
	Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath, std::vector<Uniform> p_uniforms);
	/** Constructor that initializes by compiling the shader, geometry shader and uniforms included.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	* @param p_uniforms - A vector containing uniform objects, defined just above within Shader.hpp. Used to allow single-line init.
	*/
	Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath);
	// Destructor, which deletes the shader program.
	~Shader();
	/** Compiles and sets the Shader's program ID.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	* @returns The GLuint program ID given after compilation
	*/
	GLuint compileShaders(const char* vs_filePath, const char* fs_filePath);
	/** Compiles and sets the Shader's program ID. Overload for incorporating geometry shader.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param vs_filePath - The filepath to the GLSL geometry shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	* @returns The GLuint program ID given after compilation
	*/
	GLuint compileShaders(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath);

	/// Bind the shader for use with subsequent OpenGL draw calls.
	void use();
	/// A function that handles assigning a value to a uniform within the shader's program. In this case, a boolean.
	void setBoolUniform(const std::string& p_name, bool p_value) const;
	/// A function that handles assigning a value to a uniform within the shader's program. In this case, an integer.
	void setIntUniform(const std::string& p_name, GLint p_value) const;
	/// A function that handles assigning a value to a uniform within the shader's program. In this case, a float.
	void setFloatUniform(const std::string& p_name, GLfloat p_value) const;
	/// A function that handles assigning a value to a uniform within the shader's program. In this case, an OpenGL texture.
	void setTexUniform(const std::string& p_name, GLuint p_value);
	/// A function that handles assigning a value to a uniform within the shader's program. In this case, a 4x4 matrix.
	void setMat4Uniform(const std::string& p_name, glm::mat4 p_value) const;
	// Used to set a list of uniforms using uniform objects.
	void setUniforms(std::vector<Uniform> p_uniforms);
	// Don't leak your shader programs!
	void remove();

	GLuint programID = 0; /// The OpenGL shader program ID
};

#endif