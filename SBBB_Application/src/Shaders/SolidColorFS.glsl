#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) out vec4 FragColor;

layout(location = 1) uniform vec3 col;
layout(location = 2) uniform float opacity;

in vec2 TexCoord;

void main()
{
     FragColor = vec4(col, opacity);
}