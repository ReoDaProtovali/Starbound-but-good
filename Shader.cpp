#include "Shader.hpp"
#include "GameConstants.hpp"
#include "utils.hpp"
#include <gtc/type_ptr.hpp>

Shader::Shader(const char* vs_filePath, const char* fs_filePath)
{
    programID = compileShaders(vs_filePath, fs_filePath);
}

GLuint Shader::compileShaders(const char* vs_filePath, const char* fs_filePath) {
    GLuint vs, fs, program;

    vs = glCreateShader(GL_VERTEX_SHADER);
    fs = glCreateShader(GL_FRAGMENT_SHADER);

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

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);


    glBindAttribLocation(program, attrib_position, "i_position");
    glBindAttribLocation(program, attrib_texCoord, "i_texCoord");
    glLinkProgram(program);

    glUseProgram(program);

    return program;
}

void Shader::use() {
    glUseProgram(programID);
}

void Shader::setBoolUniform(const std::string& p_name, bool p_value) const
{
    glUniform1i(glGetUniformLocation(programID, p_name.c_str()), (int)p_value);
}
void Shader::setIntUniform(const std::string& p_name, GLint p_value) const
{
    glUniform1i(glGetUniformLocation(programID, p_name.c_str()), p_value);
}
void Shader::setFloatUniform(const std::string& p_name, GLfloat p_value) const
{
    glUniform1f(glGetUniformLocation(programID, p_name.c_str()), p_value);
}

void Shader::setMat4Uniform(const std::string& p_name, glm::mat4 p_value) const
{
    glUniformMatrix4fv(glGetUniformLocation(programID, p_name.c_str()), 1, GL_FALSE, glm::value_ptr(p_value));
}