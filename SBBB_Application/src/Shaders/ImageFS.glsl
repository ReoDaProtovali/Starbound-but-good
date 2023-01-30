#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

layout(location = 1) uniform sampler2D imageTexture;

void main()
{
    vec4 col = texture(imageTexture, TexCoord);
     FragColor = col;
}