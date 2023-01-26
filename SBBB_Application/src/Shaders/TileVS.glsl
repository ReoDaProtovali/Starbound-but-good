#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in int xyzID;
layout(location = 1) in int adjacent;

out int ID;

out DATA {
	int ID;
    int adjacent;
}data_out;


uniform sampler2D tileSheet;

void main()
{
    // Unpacking method taken from TileVert struct method. There are no binary literals so I just used their decimal equivalents.
    vec3 chunkXYZ = vec3(((4227858432 & xyzID) >> 26), -float((66060288 & xyzID) >> 20), (1015808 & xyzID) >> 15);

    int aID = 32767 & xyzID;
    gl_Position = vec4(chunkXYZ, 1.0);
    data_out.ID = aID;
    data_out.adjacent = adjacent;
}