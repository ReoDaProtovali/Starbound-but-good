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
	vec2 lightingPixelUV = vec2(0.f, float(tileSheetRow) / float(tileSheetHeight)); // confusing naming
    tuv.x = 1.f - tuv.x;
    vec4 col4 = texture(tileSheet, tuv);
    //if (col4.a == 0.f) discard; // workaround, a way to get around using blending 
    //vec4 lightingInfoCol = texelFetch(tileSheet, ivec2(tileSheetRow * 24, 0), 0);
     vec4 lightingInfoCol = texture(tileSheet, lightingPixelUV);
    // scuffed lol
    if (lightingInfoCol.a != 0.f) {
    switch (int(zLevel)) {
            case 3:
            break;
            case 2:
                lightingInfoCol.a += 0.5;
            break;
            case 1:
                lightingInfoCol.a += 0.6;
            break;
            case 0:
                lightingInfoCol.a += 0.8;
            break;

        }
    }

    vec4 outCol = vec4(col4.xyz * (zLevel + 1) / 4 , col4.a);

    gl_FragData[1] = vec4(0.f, 0.f, 0.f, 0.f);

    if (!drawAsBorders) {
            gl_FragData[1] = lightingInfoCol;
        }
    gl_FragData[0] = outCol;
}