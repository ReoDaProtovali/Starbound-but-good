#version 420

// defines
#define ATTENUATION_OFFSET 0.0f
#define PI 3.141592653589
#define MAX_LIGHTS 64

// pipeline
in vec2 TexCoord;
out vec4 FragColor;

// structures
struct Ray {
    vec2 pos;
    vec2 dir;
};

struct Light {
    vec3 col;
    vec2 pos;
    float angle;
    float spread;
    int type;
};

// uniforms
uniform sampler2D lightingInfoTex;
uniform sampler2D ambientLightingTex;

layout(std140, binding = 0) uniform GlobalsBlock {
    vec4 mouse;
    vec2 screenDim;
    vec2 windowDim;
    float time_seconds;
    uint time_ticks;
};


layout(std140, binding = 1) uniform LightBlock {
    Light lights[MAX_LIGHTS];
} LIGHT_IN;

layout(std140, binding = 2) uniform DynamicLightingInfoBlock {
	vec2 topLeftTileCoord;
	vec2 tileDims;
	vec4 testLightInfo;
	uint lightCount;
};

// functions 
float hash( float x1 ) {
    int x = int (x1 * 8325.f);
    int n = x ^ 890235893;
    n = (n<<13)^n;
    n = (n*(n*n*15731+789221)+1376312589)>>16;
    return float(n%1000)/1000.f;
}

float dist2(in float x0, in float y0, in float x1, in float y1) {
    return (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
}


void stepRay(inout Ray r) {
    float slopeYX = r.dir.y / r.dir.x;
    slopeYX = clamp(slopeYX, -16384.f, 16384.f);
    float slopeXY = r.dir.x / r.dir.y;
    slopeXY = clamp(slopeXY, -16384.f, 16384.f);
    
    float sgnX = sign(r.dir.x);
    float sgnY = sign(r.dir.y);
    
    float vxDiffOrigin = ((sgnX + 1.f)/ 2.f) - mod(r.pos.x, 1.f) * sgnX;
    float vyIntOrigin = slopeYX * vxDiffOrigin;
    float vxInt = r.pos.x + vxDiffOrigin * sgnX;
    float vyInt = r.pos.y + vyIntOrigin * sgnX;

    float hyDiffOrigin = ((sgnY + 1.f)/ 2.f) - mod(r.pos.y, 1.f) * sgnY;
    float hxIntOrigin = slopeXY * hyDiffOrigin;
    float hxInt = r.pos.x + hxIntOrigin * sgnY;
    float hyInt = r.pos.y + hyDiffOrigin * sgnY;
    
    float cond = float(dist2(r.pos.x, r.pos.y, hxInt, hyInt) < dist2(r.pos.x, r.pos.y, vxInt, vyInt));
    r.pos.x = vxInt + (hxInt - vxInt) * cond + sgnX / 1024.f;
    r.pos.y = vyInt + (hyInt - vyInt) * cond + sgnY / 1024.f;
}
void stepRaySimple(inout Ray r) {
    r.pos.x += r.dir.x;
    r.pos.y += r.dir.y;
}

vec2 toShaderCoords(in vec2 p_tileCoords) {
    vec2 bottomRightTileCoord = vec2(topLeftTileCoord.x, topLeftTileCoord.y - tileDims.y);
    return (-bottomRightTileCoord + p_tileCoords) / tileDims;
}

float lightAbsorbanceSample(in vec2 tileCoord) {
    vec4 pixel = texture(lightingInfoTex, toShaderCoords(tileCoord));
    if (pixel.a == 0.f) return 1.f;
    return pixel.a;
}
vec4 lightInfoSample(in vec2 tileCoord) {
    vec4 pixel = texture(lightingInfoTex, toShaderCoords(tileCoord));
    if (pixel.a == 0.f) pixel.a = 1.f;
    return pixel;
}

void main()
{
    vec2 bottomRightTileCoord = vec2(topLeftTileCoord.x, topLeftTileCoord.y - tileDims.y);
    vec2 tileCoord;
    tileCoord.x = bottomRightTileCoord.x + tileDims.x * TexCoord.x;
    tileCoord.y = bottomRightTileCoord.y + tileDims.y * TexCoord.y;

    vec4 outCol = vec4(0.f, 0.f, 0.f, 1.f);

    for (int j = 0; j < lightCount; j++) {
        float progress = float(j + 1.f) / lightCount;
        const float timeSpeed = 0.5f;

        vec3 lightCol = LIGHT_IN.lights[j].col;
        //lightCol = testLightInfo.rgb * testLightInfo.a;
        float longestLightVal = max(lightCol.r, max(lightCol.g, lightCol.b));
        float penAdvantage = clamp(longestLightVal - 0.25, 0.f, 0.75) / 0.75;

        float lightDistance = clamp((distance(LIGHT_IN.lights[j].pos, tileCoord)) / 70.f - 0.2, 0.f, 1.f);

        vec2 rayAngle = normalize(LIGHT_IN.lights[j].pos - tileCoord);

        Ray ray = Ray(tileCoord, rayAngle);

        vec2 lightAngleVec = vec2(cos(LIGHT_IN.lights[j].angle), sin(LIGHT_IN.lights[j].angle));
        
        // a mess
        float angleFactor = (dot(
                                lightAngleVec, 
                                mix(
                                    -rayAngle, lightAngleVec, 
                                    -1.f / ((LIGHT_IN.lights[j].spread) * (LIGHT_IN.lights[j].spread)) + 2.f
                                    )
                                 ) + 1.f
                             ) / 2.f;
        
        vec3 lightFactor = lightCol * ((1.f - lightDistance)) * angleFactor;
    
        const float lightCutoff = 0.05f;
        if (lightFactor.r < lightCutoff && lightFactor.g < lightCutoff && lightFactor.b < lightCutoff) {
            FragColor = vec4(0.f, 0.f, 0.f, 1.f);
            continue;
        };

        vec2 prevRayPos = tileCoord;
        vec2 hitPoint = tileCoord;
    
        float prevAbsorbance = lightAbsorbanceSample(tileCoord);
        float absorbanceFactor = 1.f;

        for (;;) {
            if (absorbanceFactor < 0.003) {
                absorbanceFactor = 0.f;
                break;
            };
            // handle the case whenever the ray surpasses the light
            if (distance(tileCoord, ray.pos) > distance(tileCoord, LIGHT_IN.lights[j].pos)) {
                absorbanceFactor *= pow(prevAbsorbance, distance(hitPoint, LIGHT_IN.lights[j].pos) * (1.f - penAdvantage * 0.5));
                break;
            }
            
            stepRay(ray);

            vec2 midPoint = (prevRayPos + ray.pos) / 2.f;
            vec4 shrample = lightInfoSample(midPoint);
            if (shrample.a != prevAbsorbance) {
                absorbanceFactor *= pow(prevAbsorbance, distance(hitPoint, prevRayPos) * (1.f - penAdvantage * 0.5));
                prevAbsorbance = shrample.a;
                hitPoint = prevRayPos;
            }
            prevRayPos = ray.pos;
        }

        // bandaid solution
        outCol.rgb = max(lightFactor * vec3(absorbanceFactor), outCol.rgb);
    }

    FragColor = max(clamp(outCol, 0.f, 1.f), texture(ambientLightingTex, toShaderCoords(tileCoord)));
}