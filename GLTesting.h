#pragma once

#ifndef GLTESTING_H
#define GLTESTING_H

#include <stdio.h>
#include <iostream>

#define GL_GLEXT_PROTOTYPES

#include <SDL.h>
#include <SDL_opengl.h>

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

static const char* vertex_shader =
"#version 330 core\n"
"layout(location = 0) in vec2 aPos;\n"
"layout(location = 1) in vec4 aColor;\n"
"layout(location = 2) in vec2 aTexCoord;\n"

"out vec4 ourColor;\n"
"out vec2 TexCoord;\n"

"void main()\n"
"{\n"
"    gl_Position = vec4(aPos, 0.0, 1.0);\n"
"    ourColor = aColor;\n"
"    TexCoord = aTexCoord;\n"
"}\n";

static const char* fragment_shader =
"#version 330 core\n"
"out vec4 FragColor;\n"

"in vec4 ourColor;\n"
"in vec2 TexCoord;\n"

"uniform sampler2D ourTexture;\n"

"void main()\n"
"{\n"
"    FragColor = texture(ourTexture, TexCoord);\n"
"    "
"}\n";

typedef enum t_attrib_id
{
    attrib_position,
    attrib_color,
    attrib_texCoord
} t_attrib_id;

GLuint compileShaders() {
    GLuint vs, fs, program;

    vs = glCreateShader(GL_VERTEX_SHADER);
    fs = glCreateShader(GL_FRAGMENT_SHADER);

    int length = strlen(vertex_shader);
    glShaderSource(vs, 1, (const GLchar**)&vertex_shader, &length);
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

        for (int i = 0; i < errorLog.size(); i++) {
            std::cout << errorLog[i];
        }
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(vs); // Don't leak the shader.
        return -1;
    }

    length = strlen(fragment_shader);
    glShaderSource(fs, 1, (const GLchar**)&fragment_shader, &length);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fs, maxLength, &maxLength, &errorLog[0]);

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
    glBindAttribLocation(program, attrib_color, "i_color");
    glBindAttribLocation(program, attrib_texCoord, "i_texCoord");
    glLinkProgram(program);

    glUseProgram(program);

    return program;
}

void handleVertexAttrBuffers(GLuint& vao, int p_width, int p_height) {

    const GLfloat vertices[] = {
        // positions   // colors               / / texture coords
        0.5f,  0.5f,   1.0f, 0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f    // top left 
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(attrib_position, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)0);
    glVertexAttribPointer(attrib_color, 4,    GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(2 * sizeof(GLfloat)));
    glVertexAttribPointer(attrib_texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(attrib_position);
    glEnableVertexAttribArray(attrib_color);
    glEnableVertexAttribArray(attrib_texCoord);
}

#endif