#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

layout(location = 1) uniform sampler2D lightingTexture;
layout(location = 2) uniform sampler2D screenTexture;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}