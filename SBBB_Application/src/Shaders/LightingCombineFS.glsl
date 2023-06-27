#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D lightingTexture;
uniform sampler2D screenTexture;

#define LIGHT_GAIN 4.f

void main()
{
    
    float angle = 6.28318530718 / 3;
    float dist = length(TexCoord - vec2(0.5, 0.5)) * 2.f;
    vec2 rOff = dist * vec2(cos(angle * 0.0), sin(angle * 0.0)) / 800.f;
    vec2 gOff = dist * vec2(cos(angle * 1.0), sin(angle * 1.0)) / 800.f;
    vec2 bOff = dist * vec2(cos(angle * 2.0), sin(angle * 2.0)) / 800.f;
    float screenColR = texture(screenTexture, TexCoord + rOff).r;
    float screenColG = texture(screenTexture, TexCoord + gOff).g;
    float screenColB = texture(screenTexture, TexCoord + bOff).b;

    vec4 lightingCol = texture(lightingTexture, TexCoord);
    vec4 screenCol = vec4(screenColR, screenColG, screenColB, 1.f);
    FragColor = vec4(screenCol.rgb * lightingCol.rgb, 1.f); //* vec4(1.0f, 2.0f, 1.0f, 1.0f);
   
    vec4 col = (lightingCol * LIGHT_GAIN) * screenCol;
    vec3 colOverflow = clamp(col.xyz - 1.f, 0.f, 999.f);
    float avgOverflow = length(colOverflow);
    //FragColor = screenCol * lightingCol + avgOverflow * avgOverflow;
    FragColor.rgb = col.rgb + (avgOverflow * avgOverflow) * 0.25;
    //FragColor = vec4(screenColR, screenColG, screenColB, 1.f) * lightingCol;
    //vec3 col = lightingCol.rgb;

    //FragColor = vec4(lightingCol.rgb, 1.f);
    FragColor.a = 1.f;
}