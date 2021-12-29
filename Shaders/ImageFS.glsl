#version 330 core

out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D imageTexture;

void main()
{
    // Literally just takes the color of the image at TexCoord and outputs it
     FragColor = texture(imageTexture, TexCoord);
}