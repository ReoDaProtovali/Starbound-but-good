#version 330 core

layout(location = 0) out vec4 FragColor;

uniform vec3 col;
uniform float opacity;

in vec2 TexCoord;

void main()
{
     FragColor = vec4(col, opacity);
}