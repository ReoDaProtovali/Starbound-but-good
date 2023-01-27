#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tileSheet;

const int TILESHEET_WIDTH = 96;

void main()
{
    FragColor = texelFetch(tileSheet, ivec2(TILESHEET_WIDTH - TexCoord.x, TexCoord.y), 0);
    //FragColor = vec4(gl_FragCoord.xy / vec2(1280, 720), 0, 1);
}