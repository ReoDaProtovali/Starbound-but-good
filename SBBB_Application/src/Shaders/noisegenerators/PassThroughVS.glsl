#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform int seed;
uniform vec2 WorldPos;

out vec2 TexCoord;

void main()
{
    TexCoord= aTexCoord;
    gl_Position = vec4(aPos, 1.0, 1.0);
}