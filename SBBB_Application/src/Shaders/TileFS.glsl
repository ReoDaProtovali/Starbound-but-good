#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

out vec4 FragColor;

in vec2 TexCoord;
in float zLevel;

layout(location = 1) uniform int tileSheetHeight;
layout(location = 3) uniform sampler2D tileSheet;

const int TILESHEET_WIDTH = 96;

void main()
{
    vec2 tuv = vec2(TexCoord.x, TexCoord.y) / vec2(TILESHEET_WIDTH, tileSheetHeight);
    tuv.x = 1.f - tuv.x;
    vec4 col4 = texture(tileSheet, tuv);
    vec4 outCol = vec4(col4.xyz * (zLevel + 1)/2, col4.a);
    FragColor = outCol;
}