#version 330 core

out vec4 FragColor;
in vec2 FragCoord;

uniform sampler2D tileSheet;


void main()
{
    //FragColor = texture(tileSheet, TexCoord);
    if (FragCoord.x == 1.0f) {
        FragColor = vec4(0.72f,0.72f,0.72f, 1.0f);
    } else {
        FragColor = vec4(0.72f,0.55f,0.14f, 1.0f);
    }
    //FragColor = vec4(FragCoord, 0.0f, 1.0f);
}