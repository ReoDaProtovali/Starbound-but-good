#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;


layout(location = 0) uniform mat4 transform;

out vec2 TexCoord;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}