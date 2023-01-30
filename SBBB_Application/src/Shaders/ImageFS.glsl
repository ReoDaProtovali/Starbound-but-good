#version 330 core

layout(location = 0) out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D imageTexture;

void main()
{
    vec4 col = texture(imageTexture, TexCoord);
     FragColor = col;
}