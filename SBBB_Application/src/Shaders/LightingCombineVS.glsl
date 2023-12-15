#version 420

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform sampler2D lightingTexture;
uniform sampler2D screenTexture;
uniform vec2 screenRes;
uniform vec2 lightingRes;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}