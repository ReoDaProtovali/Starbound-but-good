#version 330

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D lightingInfoTex;
uniform vec2 topLeftTileCoord;
uniform vec2 tileDims;
uniform vec2 mousePos;
uniform float time;
uniform vec4 testLightInfo;

#define ATTENUATION_OFFSET 0.0f
#define PI 3.141592653589
struct Light {
    vec2 pos;
    vec3 col;
    float angle;
    float spread;
};

float hash( float x1 ) {
    int x = int (x1 * 8325.f);
    int n = x ^ 890235893;
    n = (n<<13)^n;
    n = (n*(n*n*15731+789221)+1376312589)>>16;
    return float(n%1000)/1000.f;
}


struct Ray {
    vec2 pos;
    vec2 dir;
};
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
//    FragColor = vec4(vec3(TexCoord.x, TexCoord.y, 1.f), 1.f);
//    return;

    vec4 outCol = vec4(0.f, 0.f, 0.f, 1.f);
    vec3 tinting;

    int lightCount = 1; 
    for (int j = 0; j < lightCount; j++) {
    tinting = vec3(0.f);
    float progress = float(j + 1.f) / lightCount;
    float radius = 0.f;
    const float timeSpeed = 0.5f;

    vec3 rainbow = (0.5 + 0.5*cos(time * timeSpeed * 2.f + vec3(0,2,4) + (PI * 2.f) / progress)) / 4.f;
    vec2 lightPos = mousePos + vec2(sin(time * timeSpeed + PI * 2.f * progress), cos(time * timeSpeed + PI * 2.f * progress)) * radius;
    Light testLight = Light(lightPos, testLightInfo.rgb * testLightInfo.a, 0.5f, 1.f);
    float longestLightVal = max(testLight.col.r, max(testLight.col.g, testLight.col.b));
    float penAdvantage = clamp(longestLightVal - 0.25, 0.f, 0.75) / 0.75;

    float lightDistance = clamp((distance(testLight.pos, tileCoord)) / 50.f - 0.2, 0.f, 1.f);

    vec2 rayAngle = normalize(testLight.pos - tileCoord);

    Ray ray = Ray(tileCoord, rayAngle);

    vec2 lightAngleVec = vec2(cos(testLight.angle), sin(testLight.angle));

    vec2 rayAngleInv = -rayAngle;
        
    // a mess
    float angleFactor = (dot(
                            lightAngleVec, 
                            mix(
                                rayAngleInv, lightAngleVec, 
                                -1.f / ((testLight.spread) * (testLight.spread)) + 2.f
                                )
                             ) + 1.f
                         ) / 2.f;
    
    vec3 lightFactor = testLight.col * ((1.f - lightDistance)) * angleFactor;

    const float lightCutoff = 0.05f;
    if (lightFactor.r < lightCutoff && lightFactor.g < lightCutoff && lightFactor.b < lightCutoff) {
        FragColor = vec4(0.f, 0.f, 0.f, 1.f);
        continue;
    };


    vec2 prevRayPos = tileCoord;
    vec2 hitPoint = tileCoord;

    float prevAbsorbance = lightAbsorbanceSample(tileCoord);
    float absorbanceFactor = 1.f;
    vec3 tintingFactor = vec3(1.f);
    //FragColor = vec4(vec3(prevAbsorbance), 1.f);
    //return;
    for (int i = 0; i < 70; i++) {
        if (absorbanceFactor < 0.003) {
            absorbanceFactor = 0.f;
            break;
        };
        // handle the case whenever the ray surpasses the light
        if (distance(tileCoord, ray.pos) > distance(tileCoord, testLight.pos)) {
            absorbanceFactor *= pow(prevAbsorbance, distance(hitPoint, testLight.pos) * (1.f - penAdvantage * 0.5));
            break;
        }
        
        stepRay(ray);

        vec2 midPoint = (prevRayPos + ray.pos) / 2.f;
        vec4 shrample = lightInfoSample(midPoint);
        if (shrample != prevAbsorbance) {
            absorbanceFactor *= pow(prevAbsorbance, distance(hitPoint, prevRayPos) * (1.f - penAdvantage * 0.5));
            tintingFactor *= shrample.rgb;
            prevAbsorbance = shrample.a;
            hitPoint = prevRayPos;
        }
        prevRayPos = ray.pos;
    }

    //lightFactor = tinting * length(lightFactor);
    //lightFactor +=;
        // bandaid solution
        if (tintingFactor == vec3(0.f)) tintingFactor = vec3(1.f);
        outCol.rgb = max(lightFactor * vec3(absorbanceFactor) * tintingFactor, outCol.rgb);
    }

    FragColor = clamp(outCol, 0.f, 999.f);
}