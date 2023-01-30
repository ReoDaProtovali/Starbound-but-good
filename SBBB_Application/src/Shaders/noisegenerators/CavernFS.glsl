#version 330

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float hash( int x ) {
    int n = x;
    n = (n<<13)^n;
    n = (n*(n*n*15731+789221)+1376312589)>>16;
    return float(n);
}

// Credit goes to Inigo Quilez
vec2 grad( ivec2 z )  // replace this anything that returns a random vector
{
    // 2D to 1D  (feel free to replace by some other)
    int n = z.x+z.y*11111;

    // Hugo Elias hash (feel free to replace by another one)
    n = (n<<13)^n;
    n = (n*(n*n*15731+789221)+1376312589)>>16;

#if 1

    // simple random vectors
    return vec2(cos(float(n)),sin(float(n)));
    
#else

    // Perlin style vectors
    n &= 7;
    vec2 gr = vec2(n&1,n>>1)*2.0-1.0;
    return ( n>=6 ) ? vec2(0.0,gr.x) : 
           ( n>=4 ) ? vec2(gr.x,0.0) :
                              gr;
#endif                              
}

float noise( in vec2 p )
{
    ivec2 i = ivec2(floor( p ));
     vec2 f =       fract( p );
	
	vec2 u = f*f*(3.0-2.0*f); // feel free to replace by a quintic smoothstep instead

    return (mix( mix( dot( grad( i+ivec2(0,0) ), f-vec2(0.0,0.0) ), 
                     dot( grad( i+ivec2(1,0) ), f-vec2(1.0,0.0) ), u.x),
                mix( dot( grad( i+ivec2(0,1) ), f-vec2(0.0,1.0) ), 
                     dot( grad( i+ivec2(1,1) ), f-vec2(1.0,1.0) ), u.x), u.y));
}

float fractalDistortNoise( in vec2 uv, int level, float harmonic, float amount ) {
    float currentScale = 1.f;
    vec2 result = uv;
    for (int i = 0; i < level; i++) {
        vec2 distort = vec2(noise(uv * currentScale + 84.f), noise(uv * currentScale - 64.f));
        result = result + (distort * amount) / currentScale;
        currentScale *= harmonic;
    }
    return (1.f + noise(result)) / 2.f;
}
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) out vec4 fragColor;
layout(location = 2) uniform vec2 WorldPos;

void main()
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy / vec2(512);
    uv += WorldPos;
    uv *= 10.f;
    uv = uv * vec2(1.0f, 1.5f);

    float finalNoise = pow(fractalDistortNoise(uv, 7, 3.9, 1.3f), 1.f);
    
    float cutoff = clamp(map(uv.y, 0.0f, 1.0f, 0.0f, 0.16f), 0.58, 1.0f);
    finalNoise = smoothstep(cutoff - 0.001, cutoff, finalNoise);
    vec3 col = vec3(finalNoise);

    fragColor = vec4(col,1.0);
}