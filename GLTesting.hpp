#pragma once

#ifndef GLTESTING_H
#define GLTESTING_H

#include <stdio.h>

#define GL_GLEXT_PROTOTYPES

#include <SDL.h>
#include <SDL_opengl.h>
#include "utils.hpp"
#include "glm/glm.hpp"

typedef float t_mat4x4[16];

static inline void mat4x4_ortho(t_mat4x4 out, float left, float right, float bottom, float top, float znear, float zfar)
{
#define T(a, b) (a * 4 + b)

    out[T(0, 0)] = 2.0f / (right - left);
    out[T(0, 1)] = 0.0f;
    out[T(0, 2)] = 0.0f;
    out[T(0, 3)] = 0.0f;

    out[T(1, 1)] = 2.0f / (top - bottom);
    out[T(1, 0)] = 0.0f;
    out[T(1, 2)] = 0.0f;
    out[T(1, 3)] = 0.0f;

    out[T(2, 2)] = -2.0f / (zfar - znear);
    out[T(2, 0)] = 0.0f;
    out[T(2, 1)] = 0.0f;
    out[T(2, 3)] = 0.0f;

    out[T(3, 0)] = -(right + left) / (right - left);
    out[T(3, 1)] = -(top + bottom) / (top - bottom);
    out[T(3, 2)] = -(zfar + znear) / (zfar - znear);
    out[T(3, 3)] = 1.0f;

#undef T
}


typedef enum t_attrib_id
{
    attrib_position,
    attrib_texCoord
} t_attrib_id;

GLuint compileShaders(const char* vs_filePath, const char* fs_filePath) {
    GLuint vs, fs, program;

    vs = glCreateShader(GL_VERTEX_SHADER);
    fs = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertexShader = utils::readFile(vs_filePath);
    int vsLength = vertexShader.size();
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
    int fsLength = fragmentShader.length();
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

// this is probably illegal in some courts of law
#define vec3 glm::vec3
#define vec2 glm::vec2

struct Vertex {
    Vertex(vec3 p_pos, vec2 p_texCoord) : pos(p_pos), texCoord(p_texCoord) {}
    vec3 pos;
    vec2 texCoord;
};

void handleVertexAttrBuffers(GLuint& vao, int p_width, int p_height) {

    
    const Vertex vertices1[] = {
        // positions             / / texture coords
        Vertex(vec3(0.5, 0.5, 0.0), vec2(1.0, 1.0)),   // top right
        Vertex(vec3(0.5, -0.5, 0.0), vec2(1.0, 0.0)), // bottom right
        Vertex(vec3(-0.5, -0.5, 0.0), vec2(0.0, 0.0)), // bottom left
        Vertex(vec3(-0.5, 0.5, 0.0), vec2(0.0, 1.0)) // top left
    };
    const Vertex vertices2[] = {
        // positions             / / texture coords
        Vertex(vec3(0.5, 0.5, 0.0), vec2(1.0, 1.0)),   // top right
        Vertex(vec3(0.5, -0.5, 0.0), vec2(1.0, 0.0)), // bottom right
        Vertex(vec3(-0.5, -0.5, 0.0), vec2(0.0, 0.0)), // bottom left
        Vertex(vec3(-0.5, 0.5, 0.0), vec2(0.0, 1.0)) // top left
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    glGenVertexArrays(1, &vao);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);



    glVertexAttribPointer(attrib_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(attrib_texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(attrib_position);
    glEnableVertexAttribArray(attrib_texCoord);
}

#endif