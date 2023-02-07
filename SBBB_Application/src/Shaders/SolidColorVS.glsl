#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec3 aPos;

layout(location = 0) uniform mat4 transform;
layout(location = 1) uniform vec3 col;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
}