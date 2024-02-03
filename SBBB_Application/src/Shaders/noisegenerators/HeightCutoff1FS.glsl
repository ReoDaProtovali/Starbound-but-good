#version 330

uniform int Seed;
layout(location = 0) out vec4 fragColor;
uniform vec2 WorldPos;

// Credit goes to Inigo Quilez
vec2 grad( ivec2 z )  // replace this anything that returns a random vector
{
    z.x = z.x ^ Seed;
    z.y = z.y ^ Seed;
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

    return (1.f + mix( mix( dot( grad( i+ivec2(0,0) ), f-vec2(0.0,0.0) ), 
                     dot( grad( i+ivec2(1,0) ), f-vec2(1.0,0.0) ), u.x),
                mix( dot( grad( i+ivec2(0,1) ), f-vec2(0.0,1.0) ), 
                     dot( grad( i+ivec2(1,1) ), f-vec2(1.0,1.0) ), u.x), u.y)) / 2.0f;
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

in vec2 TexCoord;


void main() {
    vec2 uv = TexCoord;
    uv += WorldPos;
    uv.y = 0.f;
    uv.x *= 0.6f / 5.f;
    vec4 value = vec4(noise(uv*10.f + 30.f)) - vec4(noise(uv*200.f + 94020.f))* 0.03 - vec4(fractalDistortNoise(uv*40.f, 2,6.f, 2.f))* 0.1f;
    value *= 5.f;
    value -= 5.f;
    if ((TexCoord.y + WorldPos.y - 1.f) * 3.f > value.x) {
      fragColor = vec4(1.f, 1.f, 1.f, 1.f);
    } else {
      fragColor = vec4(0.f, 0.f, 0.f, 1.f);
    }

}