#include "Shader.hpp"
#include "GameConstants.hpp"
#include "utils.hpp"
#include <gtc/type_ptr.hpp>

Shader::Shader(const char* vs_filePath, const char* fs_filePath)
{
	programID = Shader::compileShaders(vs_filePath, fs_filePath);
}

GLuint Shader::compileShaders(const char* vs_filePath, const char* fs_filePath) {
	GLuint vs, fs, program;

	vs = glCreateShader(GL_VERTEX_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
#ifdef LOADLOGGING_ENABLED
	std::cout << "Compiling shaders: Vertex - " << vs_filePath << " Fragment - " << fs_filePath << std::endl;
#endif
	std::string vertexShader = utils::readFile(vs_filePath);
	int vsLength = (int)vertexShader.size();
	const char* vertexShader_cstr = vertexShader.c_str();
	glShaderSource(vs, 1, (const GLchar**)&vertexShader_cstr, &vsLength);
	glCompileShader(vs);

	GLint status;

	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vs, maxLength, &maxLength, &errorLog[0]);
		std::cout << "Vertex shader compilation error" << std::endl;
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}

		glDeleteShader(vs); // Don't leak the shader.
		return -1;
	}

	std::string fragmentShader = utils::readFile(fs_filePath);
	int fsLength = (int)fragmentShader.length();
	const char* fragmentShader_cstr = fragmentShader.c_str();
	glShaderSource(fs, 1, (const GLchar**)&fragmentShader_cstr, &fsLength);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(fs, maxLength, &maxLength, &errorLog[0]);

		std::cout << "Fragment shader compilation error" << std::endl;
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}
		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(fs); // Don't leak the shader.
		return -1;
	}
#ifdef LOADLOGGING_ENABLED
	std::cout << "Shader compilation successful. Attaching to program..." << std::endl;
#endif
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);


	//glBindAttribLocation(program, 0, "i_position"); // uh oh, gonna have to un-bake this into the shader code
	//glBindAttribLocation(program, 1, "i_texCoord"); // oh sweet, it's not required if I use layout specifiers
	glLinkProgram(program);

	glUseProgram(program);

	return program;
}

void Shader::use() {
	glUseProgram(programID);
}

void Shader::setBoolUniform(const std::string& p_name, bool p_value) const
{
	GLint loc = glGetUniformLocation(programID, p_name.c_str());
#ifdef LOADLOGGING_ENABLED
	if (loc != -1) {
		std::cout << "setBoolUniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << "." << std::endl;
	}
	else {
		std::cout << "setBoolUniform: Uniform location not found for " << p_name << "." << std::endl;
	}
#endif
	glUniform1i(loc, (int)p_value);
}
void Shader::setIntUniform(const std::string& p_name, GLint p_value) const
{
	GLint loc = glGetUniformLocation(programID, p_name.c_str());
#ifdef LOADLOGGING_ENABLED
	if (loc != -1) {
		std::cout << "setIntUniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << "." << std::endl;
	}
	else {
		std::cout << "setIntUniform: Uniform location not found for " << p_name << "." << std::endl;
	}
#endif
	glUniform1i(loc, p_value);
}
void Shader::setTexUniform(const std::string& p_name, GLuint p_value)
{
	GLint loc = glGetUniformLocation(programID, p_name.c_str());
#ifdef LOADLOGGING_ENABLED
	if (loc != -1) {
		std::cout << "setTexUniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << "." << std::endl;
	}
	else {
		std::cout << "setTexUniform: Uniform location not found for " << p_name << "." << std::endl;
	}
#endif
	glUniform1i(loc, p_value);
}

void Shader::setFloatUniform(const std::string& p_name, GLfloat p_value) const
{
	GLint loc = glGetUniformLocation(programID, p_name.c_str());
#ifdef LOADLOGGING_ENABLED
	if (loc != -1) {
		std::cout << "setFloatUniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << "." << std::endl;
	}
	else {
		std::cout << "setFloatUniform: Uniform location not found for " << p_name << "." << std::endl;
	}
#endif
	glUniform1f(loc, p_value);
}
void Shader::setMat4Uniform(const std::string& p_name, glm::mat4 p_value) const
{
	GLint loc = glGetUniformLocation(programID, p_name.c_str());
#ifdef LOADLOGGING_ENABLEDD // disabled because it runs every frame
	if (loc != -1) {
		std::cout << "setMat4Uniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << "." << std::endl;
	}
	else {
		std::cout << "setMat4Uniform: Uniform location not found for " << p_name << "." << std::endl;
	}
#endif
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(p_value));
	}