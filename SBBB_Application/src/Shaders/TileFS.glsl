#version 330 core

in vec4 LightingCol;
in vec2 TexCoord;
in float z;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 InfoColor;

//in vec2 TexCoord;
//in float zLevel;
//in vec4 pos;
//in float tileSheetRow;
//in float out_ID; // for testing
//in float greyscale; // for testing

//uniform int tileSheetHeight;
uniform sampler2D tileSheet;

uniform mat4 transform;

//uniform bool drawAsBorders;

void main()
{
    //vec2 tuv = vec2(TexCoord.x, TexCoord.y) / vec2(TILESHEET_WIDTH, tileSheetHeight);
	//vec2 lightingPixelUV = vec2(0.f, float(tileSheetRow) / float(tileSheetHeight)); // confusing naming
    //tuv.x = 1.f - tuv.x;

    vec2 uv = TexCoord;
   //uv.y = 1.f - uv.y;
    //vec4 col4 = texture(tileSheet, tuv);
    //if (col4.a == 0.f) discard; // workaround, a way to get around using blending 
     vec4 lightingInfoCol = LightingCol;
    // scuffed lol
//    if (lightingInfoCol.a != 0.f) {
//    switch (int(zLevel)) {
//            case 3:
//            break;
//            case 2:
//                lightingInfoCol.a += 0.5;
//            break;
//            case 1:
//                lightingInfoCol.a += 0.6;
//            break;
//            case 0:
//                lightingInfoCol.a += 0.8;
//            break;
//
//        }
//    }

   // vec4 outCol = vec4(col4.xyz * (zLevel + 1) / 4 , col4.a);
   vec4 outCol = texture(tileSheet, uv);
   outCol.xyz *=  0.5f + (z / 3.f) / 2.f;
 //  outCol = lightingInfoCol;
    //gl_FragData[1] = vec4(0.f, 0.f, 0.f, 0.f);

    //if (!drawAsBorders) {
    //        gl_FragData[1] = lightingInfoCol;
    //    }
    FragColor = outCol;
    InfoColor = lightingInfoCol;
}