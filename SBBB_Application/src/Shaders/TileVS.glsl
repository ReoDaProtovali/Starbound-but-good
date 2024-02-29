#version 330 core

in vec3 xyz;
in vec2 texCoord;
//in uint variationCount;
//in uint flags;

out vec2 TexCoord;
out float z;

//out int ID;
uniform mat4 transform;

    //uint VariationCount;
    //uint Flags;




void main()
{
    // Unpacking method taken from TileVert struct method. There are no binary literals so I just used their decimal equivalents.
    //vec3 chunkXYZ = vec3(float((4227858432u & xyzID) >> 26), -float((66060288u & xyzID) >> 20), (1015808u & xyzID) >> 15);

    //uint aID = 32767u & xyzID;
    gl_Position = transform * vec4(xyz, 1.0);

    //data_out.ID = aID;
    // data_out.adjacent = adjacent;
    TexCoord = texCoord;
    z = xyz.z;
    //data_out.VariationCount = 0u;
   // data_out.Flags = 0u;
}