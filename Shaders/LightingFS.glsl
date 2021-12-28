#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D screenTexture;
uniform sampler2D lightingTexture;


void main()
{
     vec4 screenCol = texture(screenTexture, TexCoord);
     vec4 lightingCol = texture(lightingTexture, TexCoord);
     float centerDist = length(vec2(0.5f, 0.5f) - TexCoord.xy)*2 - 0.2f;
     FragColor = vec4(screenCol.rgb, 1.0f) * (1.0f - centerDist*centerDist); //* vec4(1.0f, 2.0f, 1.0f, 1.0f);
     //FragColor = vec4(0.0f, 0.5f, 0.0f, 1.0f);
}