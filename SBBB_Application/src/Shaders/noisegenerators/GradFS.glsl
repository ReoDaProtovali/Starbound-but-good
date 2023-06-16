#version 330

uniform int Seed;

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

layout(location = 0) out vec4 fragColor;
uniform vec2 WorldPos;

void main()
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy / vec2(512);
    uv += WorldPos;
    uv.y += 0.2;

    vec4 col = 1.f - vec4(uv.y);
    fragColor = clamp(col, 0.0f, 1.0f);
    //fragColor = vec4(col,1.0);
}