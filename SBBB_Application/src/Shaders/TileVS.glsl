#version 330 core

in vec3 xyz_i;
in vec4 lightingCol_i;
in vec2 texCoord_i;
//in uint variationCount;
//in uint flags;

out vec4 LightingCol;
out vec2 TexCoord;
out float z;

uniform mat4 transform;





void main()
{
    // Unpacking method taken from TileVert struct method. There are no binary literals so I just used their decimal equivalents.
    //vec3 chunkXYZ = vec3(float((4227858432u & xyzID) >> 26), -float((66060288u & xyzID) >> 20), (1015808u & xyzID) >> 15);

    //uint aID = 32767u & xyzID;
    gl_Position = transform * vec4(xyz_i, 1.0);

    //data_out.ID = aID;
    // data_out.adjacent = adjacent;
    TexCoord = texCoord_i;
    LightingCol = lightingCol_i;
    z = xyz_i.z;
    //data_out.VariationCount = 0u;
   // data_out.Flags = 0u;
}