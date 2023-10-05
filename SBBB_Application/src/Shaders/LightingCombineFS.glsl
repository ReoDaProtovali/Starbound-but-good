#version 420
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D lightingTexture;
uniform sampler2D screenTexture;

layout(std140, binding = 0) uniform GlobalsBlock {
    vec4 mouse;
    vec2 screenDim;
    vec2 windowDim;
    float time_seconds;
    uint time_ticks;
} g;

uniform vec2 lightingRes;

vec4 cubic(float v) {
  vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
  vec4 s = n * n * n;
  float x = s.x;
  float y = s.y - 4.0 * s.x;
  float z = s.z - 4.0 * s.y + 6.0 * s.x;
  float w = 6.0 - x - y - z;
  return vec4(x, y, z, w);
}
    
vec4 texture_bicubic(sampler2D tex, vec2 texcoord, vec2 texscale)
{
    float fx = fract(texcoord.x);
    float fy = fract(texcoord.y);
    texcoord.x -= fx;
    texcoord.y -= fy;

    vec4 xcubic = cubic(fx);
    vec4 ycubic = cubic(fy);

    vec4 c = vec4(texcoord.x - 0.5, texcoord.x + 1.5, texcoord.y - 0.5, texcoord.y + 1.5);
    vec4 s = vec4(xcubic.x + xcubic.y, xcubic.z + xcubic.w, ycubic.x + ycubic.y, ycubic.z + ycubic.w);
    vec4 offset = c + vec4(xcubic.y, xcubic.w, ycubic.y, ycubic.w) / s;

    vec4 sample0 = texture(tex, vec2(offset.x, offset.z) * texscale);
    vec4 sample1 = texture(tex, vec2(offset.y, offset.z) * texscale);
    vec4 sample2 = texture(tex, vec2(offset.x, offset.w) * texscale);
    vec4 sample3 = texture(tex, vec2(offset.y, offset.w) * texscale);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return mix(
        mix(sample3, sample2, sx),
        mix(sample1, sample0, sx), sy);
}

#define LIGHT_GAIN 4.f

vec3 abberationSample(sampler2D tex, vec2 texCoord, float offset) {
    float angle = 6.28318530718 / 3;
    //float dist = length(texCoord - vec2(0.5, 0.5)) * 0.f;
    vec2 rOff = vec2(cos(angle * 0.0), sin(angle * 0.0)) * offset;
    vec2 gOff = vec2(cos(angle * 1.0), sin(angle * 1.0)) * offset;
    vec2 bOff = vec2(cos(angle * 2.0), sin(angle * 2.0)) * offset;
    float colR = texture(tex, texCoord + rOff).r;
    float colG = texture(tex, texCoord + gOff).g;
    float colB = texture(tex, texCoord + bOff).b;
    return vec3(colR, colG, colB);
}
void main()
{
    


    //vec4 lightingCol = texture(lightingTexture, TexCoord);
    vec4 lightingCol = texture_bicubic(lightingTexture, TexCoord * lightingRes, 1.f / lightingRes);
    vec4 screenCol = texture(screenTexture, TexCoord);
   
    vec4 col = (lightingCol * LIGHT_GAIN) * screenCol;
    vec3 colOverflow = clamp(col.xyz - 1.f, 0.f, 999.f);
    float avgOverflow = length(colOverflow);

    vec3 lightOverflow = clamp((lightingCol * LIGHT_GAIN).xyz - 2.f, 0.f, 999.f);
    float avgLightOverflow = length(lightOverflow);
    screenCol = vec4(abberationSample(screenTexture, TexCoord, avgLightOverflow / 800.f), 1.f);

    col = (lightingCol * LIGHT_GAIN) * screenCol;

    FragColor.rgb = col.rgb + (avgOverflow * avgOverflow) * 0.25;

    FragColor.a = 1.f;
}