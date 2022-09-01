#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in uint xyzID;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 transform;
uniform sampler2D tileSheet;

void main()
{
    // Unpacking method taken from TileVert struct method. There are no binary literals so I just used their decimal equivalents.
    vec3 chunkXYZ = vec3(((4227858432u & xyzID) >> 26), -float((66060288u & xyzID) >> 20), (1015808u & xyzID) >> 15);

    uint aID = 32767u & xyzID;
    gl_Position = transform * model * vec4(chunkXYZ, 1.0);
    float fID = float(aID);
    TexCoord = vec2((sin(fID / 2) + 1) / 2, (cos(fID / 2) + 1) / 2);
}