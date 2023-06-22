#version 330 core

layout(location = 0) out vec4 FragColor;

uniform sampler2D lightingInfoTex;
uniform vec2 inputResolution; // lighting info resolution
uniform vec2 topLeftTileCoord;
uniform vec2 tileDims;
uniform vec2 mousePos;

in vec2 TexCoord;

#define PI 3.14159265f
#define TAU 6.28318530718

#define ATTENUATION_OFFSET 0.0f
struct Light {
    vec2 pos;
    vec3 col;
    float angle;
    float spread;
};

struct Ray {
    vec2 pos;
    vec2 dir;
};
float dist2(in float x0, in float y0, in float x1, in float y1) {
    return (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
}

void stepRay(inout Ray r) {
    float slopeYX = r.dir.y / r.dir.x;
    float slopeXY = r.dir.x / r.dir.y;
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
    
    float distH = dist2(r.pos.x, r.pos.y, hxInt, hyInt);
    float distV = dist2(r.pos.x, r.pos.y, vxInt, vyInt);
    float cond = float(distH < distV);
    r.pos.x = vxInt + (hxInt - vxInt) * cond + sgnX / 128.f;
    r.pos.y = vyInt + (hyInt - vyInt) * cond + sgnY / 128.f;
}

vec2 toShaderCoords(in vec2 p_tileCoords) {
    vec2 bottomRightTileCoord = vec2(topLeftTileCoord.x, topLeftTileCoord.y - tileDims.y);
    return (-bottomRightTileCoord + p_tileCoords) / tileDims;
}

float lightInfoSample(vec2 tileCoord) {
    vec4 pixel = texture(lightingInfoTex, toShaderCoords(tileCoord));
    if (pixel.r == 1.f) return 1.f;
    return 1.f-pixel.r;
}
void main()
{
    vec2 bottomRightTileCoord = vec2(topLeftTileCoord.x, topLeftTileCoord.y - tileDims.y);
    vec2 tileCoord;
    tileCoord.x = bottomRightTileCoord.x + tileDims.x * TexCoord.x;
    tileCoord.y = bottomRightTileCoord.y + tileDims.y * TexCoord.y;

    Light testLight = Light(mousePos, vec3(2.f, 1.0f, 0.5f), 1.f, 1.f);

    vec4 outCol;

    //vec2 normalizedLightPos = vec2(mouseLight.pos.x * aspect, mouseLight.pos.y) / inputResolution;

    float lightDistance = length(testLight.pos - tileCoord) / 200.f;

    float rayAngle = atan(testLight.pos.y - tileCoord.y, testLight.pos.x - tileCoord.x);

    Ray ray = Ray(tileCoord, vec2(cos(rayAngle), sin(rayAngle)));

    vec2 prevRayPos = ray.pos;
    vec2 hitPoint = ray.pos;

    float prevAbsorbance = lightInfoSample(ray.pos);
    float absorbanceFactor = 1.f;

    //FragColor = vec4(vec3(prevAbsorbance), 1.f);
    //return;
    float count = 0;
    for (int i = 0; i < 100; i++) {
        // handle the case whenever the ray surpasses the light
        if (distance(tileCoord, ray.pos) > distance(tileCoord, testLight.pos)) {
            absorbanceFactor *= pow(mix(prevAbsorbance, 1.0f, ATTENUATION_OFFSET), distance(hitPoint, testLight.pos));
            break;
        }
        
        stepRay(ray);

        vec2 midPoint = (prevRayPos + ray.pos) / 2.f;
        float shrample = lightInfoSample(midPoint);
        if (shrample != prevAbsorbance) {
            absorbanceFactor *= pow(mix(prevAbsorbance, 1.f, ATTENUATION_OFFSET), distance(hitPoint, prevRayPos));
            prevAbsorbance = shrample;
            count += 1.f;
            hitPoint = prevRayPos;
        }
        prevRayPos = ray.pos;
    }

    // Fade rays nearing the end so there's not a sharp transition
    float normalizedDist = clamp(1.f - lightDistance, 0.f, 1.f);
    
    vec2 lightAngleVec = vec2(cos(testLight.angle), sin(testLight.angle));
    // aspect code is weird
    vec2 rayAngleVec = -vec2(ray.dir.x, ray.dir.y);
        
    // a mess
    float angleFactor = (dot(
                            lightAngleVec, 
                            mix(
                                rayAngleVec, lightAngleVec, 
                                -1.f / ((testLight.spread) * (testLight.spread)) + 2.f
                                )
                             ) + 1.f
                         ) / 2.f;   
    
    vec3 lightFactor = testLight.col * normalizedDist * angleFactor;
    outCol.rgb = lightFactor * absorbanceFactor;
    outCol.a = 1.f;
//    float prevAbsorbance = 1.f-texture(lightingInfoTex, toShaderCoords(ray.pos)).r;
//    float absorbanceFactor = 1.f;
//
//    float count = 0.f;
//    for (int i = 0; i < 200; i++) {
//        if (distance(tileCoord, ray.pos) > distance(tileCoord, testLight.pos)) {
//            count += 1.f;
//            break;
//        }
//        stepRay(ray);
//
//        vec2 midPoint = (prevRayPos + vec2(cos(ray.dir.x), sin(ray.dir.y)) * 0.01f);
//        float shrample = texture(lightingInfoTex, toShaderCoords(prevRayPos)).r;
//        count += shrample;
//        prevRayPos = ray.pos;
//    }
    //FragColor = vec4(vec3(absorbanceFactor), 1.f);
    //outCol.rgb = 1.f-vec3(count / 10.f);
    FragColor = clamp(outCol, 0.f, 999.f);
}