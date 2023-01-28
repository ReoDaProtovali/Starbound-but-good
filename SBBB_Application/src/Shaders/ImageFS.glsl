#version 330 core

layout(location = 0) out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D imageTexture;

uniform float bungus;

void main()
{
    // Literally just takes the color of the image at TexCoord and outputs it
    vec4 col = texture(imageTexture, TexCoord);
    //if (col.a == 0.0f) discard;
     FragColor = col;
}