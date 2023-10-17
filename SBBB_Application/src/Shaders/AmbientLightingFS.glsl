#version 420

// pipeline
in vec2 TexCoord;
out vec4 FragColor;

// uniforms
uniform sampler2D ambientLightingTex;

layout(std140, binding = 0) uniform GlobalsBlock {
    vec4 mouse;
    vec2 screenDim;
    vec2 windowDim;
    float time_seconds;
    uint time_ticks;
};

layout(std140, binding = 3) uniform AmbientLightingInfoBlock {
	vec2 topLeftTileCoord;
	vec2 tileDims;
};

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

vec2 toShaderCoords(in vec2 p_tileCoords) {
    vec2 bottomRightTileCoord = vec2(topLeftTileCoord.x, topLeftTileCoord.y - tileDims.y);
    return (-bottomRightTileCoord + p_tileCoords) / tileDims;
}


void main()
{
    vec2 bottomRightTileCoord = vec2(topLeftTileCoord.x, topLeftTileCoord.y - tileDims.y);
    vec2 tileCoord;
    tileCoord.x = bottomRightTileCoord.x + tileDims.x * TexCoord.x;
    tileCoord.y = bottomRightTileCoord.y + tileDims.y * TexCoord.y;

    //FragColor = texture_bicubic(ambientLightingTex, toShaderCoords(tileCoord) * tileDims - vec2(0.5f), 1.f / tileDims);
    FragColor = texture(ambientLightingTex, toShaderCoords(tileCoord));
}