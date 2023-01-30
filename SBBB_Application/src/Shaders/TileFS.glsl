#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

out vec4 FragColor;

in vec2 TexCoord;

layout(location = 3) uniform sampler2D tileSheet;

const int TILESHEET_WIDTH = 96;

void main()
{
    FragColor = texelFetch(tileSheet, ivec2(TILESHEET_WIDTH - TexCoord.x, TexCoord.y), 0);
}