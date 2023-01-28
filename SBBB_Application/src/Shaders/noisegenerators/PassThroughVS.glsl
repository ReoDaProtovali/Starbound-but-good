#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

layout(location = 2) uniform vec2 WorldPos;

out vec2 TexCoord;

void main()
{
    TexCoord= aTexCoord;
    gl_Position = vec4(aPos, 1.0, 1.0);
}