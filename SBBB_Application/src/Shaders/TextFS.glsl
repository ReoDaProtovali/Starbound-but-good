#version 330 core

#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) out vec4 FragColor;
layout(location = 0) uniform sampler2D fontAtlas;
layout(location = 1) uniform vec3 textCol;

in vec2 TexCoord;

void main()
{
    vec4 col = texture(fontAtlas, TexCoord);

    FragColor = vec4(textCol, col.r);
    //FragColor = vec4(0, 0, 0, 1);
}