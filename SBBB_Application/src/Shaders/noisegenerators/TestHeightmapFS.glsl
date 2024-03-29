#version 330
uniform int Seed;

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


layout(location = 0) out vec4 fragColor;

uniform vec2 WorldPos;

in vec2 TexCoord;

void main() {
    vec2 uv = vec2(TexCoord.x, 0.0f);
    uv.x += WorldPos.x; 
    fragColor = vec4(noise(uv*10.f + 30.f)) - vec4(noise(uv*100.f + 94020.f))* 0.07;
}