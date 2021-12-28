#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D screenTexture;
uniform sampler2D lightingTexture;


void main()
{
     vec4 screenCol = texture(screenTexture, TexCoord);
     vec4 lightingCol = texture(lightingTexture, TexCoord);
     FragColor = screenCol; //* sin(TexCoord.x * 5); //* vec4(1.0f, 2.0f, 1.0f, 1.0f);
     //FragColor = vec4(0.0f, 0.5f, 0.0f, 1.0f);
}