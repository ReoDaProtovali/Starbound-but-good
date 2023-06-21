#version 330 core


uniform int tileSheetHeight;
uniform sampler2D tileSheet;

in float zLevel;
in vec2 TexCoord;
in float tileSheetRow;
const int TILESHEET_WIDTH = 96;

void main()
{
	vec2 tuv = vec2(0.f, tileSheetRow);
    //tuv.x = 1.f - tuv.x;
    vec4 col4 = texture(tileSheet, tuv);
    vec4 outCol = vec4(col4.xyz, col4.a);
}