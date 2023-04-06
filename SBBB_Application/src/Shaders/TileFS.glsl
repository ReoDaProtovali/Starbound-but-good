#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
out vec4 FragColor;

in vec2 TexCoord;
in float zLevel;
in float out_ID; // for testing
in float greyscale; // for testing

layout(location = 1) uniform int tileSheetHeight;
layout(location = 3) uniform sampler2D tileSheet;

const int TILESHEET_WIDTH = 96;

void main()
{
    vec2 tuv = vec2(TexCoord.x, TexCoord.y) / vec2(TILESHEET_WIDTH, tileSheetHeight);
    tuv.x = 1.f - tuv.x;
    vec4 col4 = texture(tileSheet, tuv);
    vec4 outCol = vec4(col4.xyz * (zLevel + 1) / 4 , col4.a);

    if (out_ID == 32767.f) {
        FragColor = vec4(vec3(greyscale), 1.f);
        return;
    }
    FragColor = outCol;
    //FragColor = vec4(0, 0, 0, 1);
}