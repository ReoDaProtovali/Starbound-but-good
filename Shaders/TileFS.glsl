#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D tileSheet;


void main()
{
    // Literally just takes the color of the image at TexCoord and outputs it
    //FragColor = texture(tileSheet, TexCoord);
    FragColor = vec4(TexCoord, 0.0f, 0.0f);
}