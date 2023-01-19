#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D lightingTexture;
uniform sampler2D screenTexture;



void main()
{
     vec4 screenCol = texture(screenTexture, TexCoord);
     vec4 lightingCol = texture(lightingTexture, TexCoord);
     //FragColor = vec4(screenCol.rgb + lightingCol.xyz, 1.0f); //* vec4(1.0f, 2.0f, 1.0f, 1.0f);
     FragColor = screenCol * lightingCol;
     //FragColor = lightingCol;
}