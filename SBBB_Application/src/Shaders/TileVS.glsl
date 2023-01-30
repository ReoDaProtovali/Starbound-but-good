#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in uint xyzID;
layout(location = 1) in uint adjacent;
layout(location = 2) in uint variationCount;

//out int ID;

out DATA {
	uint ID;
    uint adjacent;
    uint variationCount;
}data_out;


uniform sampler2D tileSheet;

void main()
{
    // Unpacking method taken from TileVert struct method. There are no binary literals so I just used their decimal equivalents.
    vec3 chunkXYZ = vec3(float((4227858432u & xyzID) >> 26), -float((66060288u & xyzID) >> 20), (1015808u & xyzID) >> 15);

    uint aID = 32767u & xyzID;
    gl_Position = vec4(chunkXYZ, 1.0);
    data_out.ID = aID;
    data_out.adjacent = adjacent;
    data_out.variationCount = variationCount;
}