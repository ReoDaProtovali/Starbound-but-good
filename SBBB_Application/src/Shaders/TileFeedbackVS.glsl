#version 330 core

layout(location = 0) in vec2 a_TexCoord;
layout(location = 1) in float a_zLevel;
layout(location = 2) in vec4 a_pos;

// needs to be here or else it will yell at me
uniform mat4 transform;

out vec2 TexCoord;
out float zLevel;
out vec4 pos;

void main()
{
    TexCoord = a_TexCoord;
    zLevel = a_zLevel;
    pos = a_pos;
    gl_Position = transform * a_pos;
    gl_Position.w = 1.f;
}