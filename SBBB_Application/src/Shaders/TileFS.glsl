#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

out vec4 FragColor;

in vec2 TexCoord;
in float zLevel;

layout(location = 3) uniform sampler2D tileSheet;

const int TILESHEET_WIDTH = 96;

void main()
{
    vec4 col4 = texelFetch(tileSheet, ivec2(TILESHEET_WIDTH - TexCoord.x, TexCoord.y), 0);
    vec4 outCol = vec4(col4.xyz * (zLevel + 1)/2, col4.a);
    FragColor = outCol;
}