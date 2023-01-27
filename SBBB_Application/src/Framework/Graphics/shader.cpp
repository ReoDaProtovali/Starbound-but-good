#include "Framework/Graphics/Shader.hpp"
#include "Framework/Graphics/GlCheck.hpp"
#include "util/utils.hpp"
#include <util/ext/glm/gtc/type_ptr.hpp>
#include <exception>

Shader::Shader(const char* vs_filePath, const char* fs_filePath)
{
	program->ID = Shader::compileShaders(vs_filePath, fs_filePath);
}

Shader::Shader(const char* vs_filePath, const char* fs_filePath, std::vector<Uniform> p_uniforms)
{
	program->ID = Shader::compileShaders(vs_filePath, fs_filePath);
	Shader::setUniforms(p_uniforms);
}

Shader::Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath)
{
	program->ID = Shader::compileShaders(vs_filePath, gs_filePath, fs_filePath);
}
Shader::Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath, std::vector<Uniform> p_uniforms)
{
	program->ID = Shader::compileShaders(vs_filePath, gs_filePath, fs_filePath);
	Shader::setUniforms(p_uniforms);
}

Shader& Shader::operator=(const Shader& p_other) {
	program = p_other.program;
	return *this;
}

Shader::Shader(Shader&& other) noexcept {
	LOG("Moved Shader! Program ID: " << other.program->ID);
	other.program = std::move(program);
}
GLuint Shader::compileShaders(const char* vs_filePath, const char* fs_filePath) {
	GLuint vs, fs;

	vs = glCreateShader(GL_VERTEX_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);

	LOAD_LOG("Compiling shaders: Vertex - " << vs_filePath << " Fragment - " << fs_filePath);

	std::string vertexShader = utils::readFile(vs_filePath);
	GLint vsLength = (GLint)vertexShader.size();
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
		LOG("Vertex shader compilation error");
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}

		glCheck(glDeleteShader(vs)); // Don't leak the shader.

		throw new std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	std::string fragmentShader = utils::readFile(fs_filePath);
	GLint fsLength = (GLint)fragmentShader.length();
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

		LOG("Fragment shader compilation error" << std::endl);
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}
		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glCheck(glDeleteShader(fs)); // Don't leak the shader.
		throw new std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	LOAD_LOG("Shader compilation successful. Attaching to program...");

	program->ID = glCreateProgram();
	glCheck(glAttachShader(program->ID, vs));
	glCheck(glAttachShader(program->ID, fs));

	glCheck(glDeleteShader(vs));
	glCheck(glDeleteShader(fs));

	glCheck(glLinkProgram(program->ID));
	
	glCheck(glUseProgram(program->ID));

	return program->ID;
}

GLuint Shader::compileShaders(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath)
{
	GLuint vs, gs, fs;

	// Throwaway
	vs = glCreateShader(GL_VERTEX_SHADER);
	gs = glCreateShader(GL_GEOMETRY_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);

	LOAD_LOG("Compiling shaders: Vertex - " << vs_filePath << " Geometry - " << gs_filePath << " Fragment - " << fs_filePath);

	std::string vertexShader = utils::readFile(vs_filePath);
	GLint vsLength = (GLint)vertexShader.size();
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
		LOG("Vertex shader compilation error");
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}

		glDeleteShader(vs); // Don't leak the shader.

		throw new std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	std::string geometryShader = utils::readFile(gs_filePath);
	GLint gsLength = (GLint)geometryShader.size();
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
		LOG("Geometry shader compilation error");
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}

		glDeleteShader(gs); // Don't leak the shader.

		throw new std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	std::string fragmentShader = utils::readFile(fs_filePath);
	GLint fsLength = (GLint)fragmentShader.length();
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

		LOG("Fragment shader compilation error");
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}
		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(fs); // Don't leak the shader.
		throw new std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	LOAD_LOG("Shader compilation successful. Attaching to program...");

	program->ID = glCreateProgram();
	glAttachShader(program->ID, vs);
	glAttachShader(program->ID, gs);
	glAttachShader(program->ID, fs);

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);

	glLinkProgram(program->ID);

	glUseProgram(program->ID);

	return program->ID;
}

void Shader::use() const { 
	glUseProgram(program->ID);
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
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.c_str());
#ifdef LOADLOGGING_ENABLED
	if (loc != -1) {
		LOAD_LOG("setBoolUniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << ".");
	}
	else {
		LOAD_LOG("setBoolUniform: Uniform location not found for " << p_name << ".");
	}
#endif
	glUniform1i(loc, (int)p_value);
}
void Shader::setBoolUniform(GLint p_loc, bool p_value) const
{
	use();
	glUniform1i(p_loc, (int)p_value);
}
void Shader::setIntUniform(const std::string& p_name, GLint p_value) const
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.c_str());
#ifdef LOADLOGGING_ENABLED
	if (loc != -1) {
		LOAD_LOG("setIntUniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << ".");
	}
	else {
		LOAD_LOG("setIntUniform: Uniform location not found for " << p_name << ".");
	}
#endif
	glUniform1i(loc, p_value);
}
void Shader::setIntUniform(GLint p_loc, GLint p_value) const
{
	use();
	glUniform1i(p_loc, p_value);
}
void Shader::setTexUniform(const std::string& p_name, GLuint p_value)
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.c_str());
#ifdef LOADLOGGING_ENABLED
	if (loc != -1) {
		LOAD_LOG("setTexUniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << ".");
	}
	else {
		LOAD_LOG("setTexUniform: Uniform location not found for " << p_name << ".");
	}
#endif
	glUniform1i(loc, p_value);
}

void Shader::setTexUniform(GLint p_loc, GLuint p_value)
{
	use();
	glUniform1i(p_loc, p_value);
}

void Shader::setFloatUniform(const std::string& p_name, GLfloat p_value) const
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.c_str());
#ifdef LOADLOGGING_ENABLED
	if (loc != -1) {
		LOAD_LOG("setFloatUniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << ".");
	}
	else {
		LOAD_LOG("setFloatUniform: Uniform location not found for " << p_name << ".");
	}
#endif
	glUniform1f(loc, p_value);
}
void Shader::setFloatUniform(GLint p_loc, GLfloat p_value) const
{
	use();
	glUniform1f(p_loc, p_value);
}
void Shader::setMat4Uniform(const std::string& p_name, glm::mat4 p_value) const
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.c_str());
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

void Shader::setMat4Uniform(GLint p_loc, glm::mat4 p_value) const
{
	use();
	glUniformMatrix4fv(p_loc, 1, GL_FALSE, glm::value_ptr(p_value));
}

void Shader::setVec2Uniform(const std::string& p_name, glm::vec2 p_value) const 
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.c_str());
#ifdef LOADLOGGING_ENABLED
	if (loc != -1) {
		LOAD_LOG("setVec2Uniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << ".");
	}
	else {
		LOAD_LOG("setVec2Uniform: Uniform location not found for " << p_name << ".");
	}
#endif
	glUniform2f(loc, p_value.x, p_value.y);
}

void Shader::setVec2Uniform(GLint p_loc, glm::vec2 p_value) const
{
	use();
	glUniform2f(p_loc, p_value.x, p_value.y);
}
