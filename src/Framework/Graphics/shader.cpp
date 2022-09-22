#include "Framework\Graphics\Shader.hpp"
#include "GameConstants.hpp"
#include "Framework/Graphics/GlCheck.hpp"
#include "util/utils.hpp"
#include <util/ext/glm/gtc/type_ptr.hpp>
#include <exception>

Shader::Shader(const char* vs_filePath, const char* fs_filePath)
{
	programID = Shader::compileShaders(vs_filePath, fs_filePath);
}

Shader::Shader(const char* vs_filePath, const char* fs_filePath, std::vector<Uniform> p_uniforms)
{
	programID = Shader::compileShaders(vs_filePath, fs_filePath);
	Shader::setUniforms(p_uniforms);
}

Shader::Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath)
{
	programID = Shader::compileShaders(vs_filePath, gs_filePath, fs_filePath);

}
Shader::Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath, std::vector<Uniform> p_uniforms)
{
	programID = Shader::compileShaders(vs_filePath, gs_filePath, fs_filePath);
	Shader::setUniforms(p_uniforms);
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
	glCheck(glShaderSource(vs, 1, (const GLchar**)&vertexShader_cstr, &vsLength));
	glCheck(glCompileShader(vs));

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

		glCheck(glDeleteShader(vs)); // Don't leak the shader.

		throw new std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	std::string fragmentShader = utils::readFile(fs_filePath);
	int fsLength = (int)fragmentShader.length();
	const char* fragmentShader_cstr = fragmentShader.c_str();
	glCheck(glShaderSource(fs, 1, (const GLchar**)&fragmentShader_cstr, &fsLength));
	glCheck(glCompileShader(fs));

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
		glCheck(glDeleteShader(fs)); // Don't leak the shader.
		throw new std::runtime_error("Shader was unable to compile.");
		return -1;
	}
#ifdef LOADLOGGING_ENABLED
	std::cout << "Shader compilation successful. Attaching to program..." << std::endl;
#endif
	program = glCreateProgram();
	glCheck(glAttachShader(program, vs));
	glCheck(glAttachShader(program, fs));

	glCheck(glDeleteShader(vs));
	glCheck(glDeleteShader(fs));

	glCheck(glLinkProgram(program));

	glCheck(glUseProgram(program));

	return program;
}

GLuint Shader::compileShaders(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath)
{
	GLuint vs, gs, fs, program;

	vs = glCreateShader(GL_VERTEX_SHADER);
	gs = glCreateShader(GL_GEOMETRY_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
#ifdef LOADLOGGING_ENABLED
	std::cout << "Compiling shaders: Vertex - " << vs_filePath << " Geometry - " << gs_filePath << " Fragment - " << fs_filePath << std::endl;
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

		throw new std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	std::string geometryShader = utils::readFile(gs_filePath);
	int gsLength = (int)geometryShader.size();
	const char* geometryShader_cstr = geometryShader.c_str();
	glShaderSource(gs, 1, (const GLchar**)&geometryShader_cstr, &gsLength);
	glCompileShader(gs);


	glGetShaderiv(gs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(gs, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(gs, maxLength, &maxLength, &errorLog[0]);
		std::cout << "Geometry shader compilation error" << std::endl;
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}

		glDeleteShader(gs); // Don't leak the shader.

		throw new std::runtime_error("Shader was unable to compile.");
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
		throw new std::runtime_error("Shader was unable to compile.");
		return -1;
	}
#ifdef LOADLOGGING_ENABLED
	std::cout << "Shader compilation successful. Attaching to program..." << std::endl;
#endif
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, gs);
	glAttachShader(program, fs);

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);

	glLinkProgram(program);

	glUseProgram(program);

	return program;
}

void Shader::use() {
	glUseProgram(programID);
}

void Shader::setUniforms(std::vector<Uniform> p_uniforms) {
	for (size_t i = 0; i < p_uniforms.size(); i++) {
		switch (p_uniforms[i].type) {
		case UniformTypes::TEX:
			Shader::setTexUniform(p_uniforms[i].name, p_uniforms[i].intValue);
			break;
		case UniformTypes::INT:
			Shader::setIntUniform(p_uniforms[i].name, p_uniforms[i].intValue);
			break;
		case UniformTypes::BOOL:
			Shader::setBoolUniform(p_uniforms[i].name, (bool)p_uniforms[i].intValue);
			break;
		case UniformTypes::FLOAT:
			Shader::setFloatUniform(p_uniforms[i].name, p_uniforms[i].floatValue);
			break;
		case UniformTypes::MAT4:
			Shader::setMat4Uniform(p_uniforms[i].name, p_uniforms[i].mat4Value);
			break;
		default:
			throw new std::runtime_error("Attempted to set uniform with invalid type.");
		}
	}
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