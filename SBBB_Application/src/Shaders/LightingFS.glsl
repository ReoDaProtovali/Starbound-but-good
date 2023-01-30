#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

out vec4 FragColor;
in vec2 TexCoord;

layout(location = 1) uniform sampler2D lightingTexture;
layout(location = 2) uniform sampler2D screenTexture;



void main()
{
     vec4 screenCol = texture(screenTexture, TexCoord);
     vec4 lightingCol = texture(lightingTexture, TexCoord);
     //FragColor = vec4(screenCol.rgb + lightingCol.xyz, 1.0f); //* vec4(1.0f, 2.0f, 1.0f, 1.0f);
     FragColor = screenCol * lightingCol;
     //FragColor = lightingCol;
}