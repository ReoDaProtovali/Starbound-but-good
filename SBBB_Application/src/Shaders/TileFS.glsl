#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in float zLevel;
in vec4 pos;
in float tileSheetRow;
//in float out_ID; // for testing
//in float greyscale; // for testing

uniform int tileSheetHeight;
uniform sampler2D tileSheet;

const int TILESHEET_WIDTH = 96;
uniform bool drawAsBorders;

void main()
{
    vec2 tuv = vec2(TexCoord.x, TexCoord.y) / vec2(TILESHEET_WIDTH, tileSheetHeight);
	vec2 lightingPixelUV = vec2(0.f, tileSheetRow / tileSheetHeight); // confusing naming
    tuv.x = 1.f - tuv.x;
    vec4 col4 = texture(tileSheet, tuv);
    vec4 lightingInfoCol = texture(tileSheet, lightingPixelUV);
    lightingInfoCol.a *= pow((zLevel + 1) / 4, 3);

    vec4 outCol = vec4(col4.xyz * (zLevel + 1) / 4 , col4.a);

    //if (out_ID == 32767.f) {
    //    FragColor = vec4(vec3(greyscale), 1.f);
    //    return;
    //}
    gl_FragData[0] = outCol;
    gl_FragData[1] = vec4(0.f);
    if (!drawAsBorders) {
            lightingInfoCol.r = lightingInfoCol.w; // at some point it gets alpha blended and i have no idea where, have to do this
            //lightingInfoCol.gb = vec2(0.0);
            //lightingInfoCol.w = 1.f;
            gl_FragData[1] = lightingInfoCol;
        }
    //FragColor = vec4(0, 0, 0, 1);
}