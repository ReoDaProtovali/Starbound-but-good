#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout(location = 0) out vec4 FragColor;
in vec2 TexCoord;

layout(location = 1) uniform sampler2D imageTexture;
layout(location = 2) uniform float opacity;
void main()
{
    vec4 col = texture(imageTexture, TexCoord);
    //col = vec4(0,0,0,1);
    FragColor = col - vec4(0.f, 0.f, 0.f, opacity);// + vec4(0.2, 0, 0, 1);
}