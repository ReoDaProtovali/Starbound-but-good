#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout (points) in;
layout (triangle_strip, max_vertices = 36) out;

layout(location = 0) uniform mat4 transform;
layout(location = 1) uniform int tileSheetHeight;
layout(location = 2) uniform vec2 worldPos;

out vec2 TexCoord;


in DATA {
	uint ID;
    uint adjacent;
    uint variationCount;
}data_in[];

const uint SPRITE_WIDTH = 16u;
const uint SPRITE_HEIGHT = 24u;

bool[8] uintToBoolArr(in uint val) {
	bool[8] result;
    for (int i = 0; i < 8; i++) {
        result[i] = bool((val >> i) & 1u);
    }
	return result;
};


float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 IDToTexOffset(uint ID) {
	// it has to be backwards because tiles are appended in reverse order ugh
	return vec2(0, uint(tileSheetHeight) - SPRITE_HEIGHT * (ID + 1u));
}
// uhhhhh texbounds is (x1, y1, x2, y2) while regionbounds is (x1, y1, w, h)
// warning i think the origin is at the bottom left and that's kinda stupid
void pushRegion(vec4 texBounds, vec4 regionBounds, float z) {
	uint randp = abs(uint(float(18729) * rand(gl_in[0].gl_Position.xy + worldPos * 2.f))) % data_in[0].variationCount;
	vec2 offset =  IDToTexOffset(data_in[0].ID) + vec2(SPRITE_WIDTH * randp, 0);
	// boo flickering
	regionBounds += vec4(-0.001f, -0.001f, 0.0025f, 0.0025f);
	gl_Position = transform * (gl_in[0].gl_Position + vec4(regionBounds.x, regionBounds.y, z, 0));
	TexCoord = vec2(texBounds.xw + offset);
	EmitVertex();

	gl_Position = transform * (gl_in[0].gl_Position + vec4(regionBounds.x + regionBounds.z, regionBounds.y, z, 0));
	TexCoord = vec2(texBounds.zw + offset);
	EmitVertex();

	gl_Position = transform * (gl_in[0].gl_Position + vec4(regionBounds.x, regionBounds.y + regionBounds.w, z, 0));
	TexCoord = vec2(texBounds.xy + offset);
	EmitVertex();

	gl_Position = transform * (gl_in[0].gl_Position + vec4(regionBounds.x + regionBounds.z, regionBounds.y + regionBounds.w, z, 0));
	TexCoord = vec2(texBounds.zy + offset);
	EmitVertex();

	EndPrimitive();
}
void main() {
	// i kinda messed up the indexing on this thing, it's funky
	// tile layout:
	//7 6 5
	//4 # 3
	//2 1 0
	// true means the tile is NOT occupied
	bool adj[8] = uintToBoolArr(data_in[0].adjacent);

	pushRegion(vec4(4, 12, 12, 20), vec4(0, -1, 1, 1), -1);

	// If there are no tiles above, right, and top right
	if (adj[6] && adj[5] && adj[3]) {
		pushRegion(vec4(12, 8, 16, 12), vec4(1.0, 0.0, 0.5, 0.5), 0);
	}

    // If there are no tiles above, left, and top left
	if (adj[6] && adj[4] && adj[7]) {
		pushRegion(vec4(0, 8, 4, 12), vec4(-0.5, 0.0, 0.5, 0.5), 0);
	}

	// If there are no tiles below, left, and bottom left
	if (adj[1] && adj[4] && adj[2]) {
		pushRegion(vec4(0, 20, 4, 24), vec4(-0.5, -1.5, 0.5, 0.5), 0);
	}

    // If there are no tiles below, right, and bottom right
	if (adj[1] && adj[3] && adj[0]) {
		pushRegion(vec4(12, 20, 16, 24), vec4(1.0, -1.5, 0.5, 0.5), 0);
	}

	// If there are no tiles above
	if (adj[6]) {
		pushRegion(vec4(4, 8, 12, 12), vec4(0.0, 0.0, 1.0, 0.5), 0);
	}
	// If there are no tiles below
	if (adj[1]) {
		pushRegion(vec4(4, 20, 12, 24), vec4(0.0, -1.5, 1.0, 0.5), 0);
	}
	// If there are no tiles left
	if (adj[4]) {
		pushRegion(vec4(0, 12, 4, 20), vec4(-0.5, -1.0, 0.5, 1.0), 0);
	}
	// If there are no tiles right
	if (adj[3]) {
		pushRegion(vec4(12, 12, 16, 20), vec4(1.0, -1.0, 0.5, 1.0), 0);
	}
	// Inner Corners ---------------------------------------------------
	// 1: bottom
	// 4: left
	// 2: bottom left
	if (!adj[1] && !adj[4] && adj[2]) {
		pushRegion(vec4(4, 0, 8, 4), vec4(-0.5, -1.5, 0.5, 0.5), 1.f);
	}

	// 4: left
	// 6: top
	// 7: top left
	if (!adj[4] && !adj[6] && adj[7]) {
		//const vec4 IC = vec4(4, 4, 8, 8);
		pushRegion(vec4(4, 4, 8, 8), vec4(-0.5, 0.0, 0.5, 0.5), 1.f);

	}

	// 3: right
	// 1: bottom
	// 0: bottom right
	if (!adj[3] && !adj[1] && adj[0]) {
		pushRegion(vec4(0, 0, 4, 4), vec4(1.0, -1.5, 0.5, 0.5), 1.f);
	}

	// 6: top
	// 3: right
	// 5: top right
	if (!adj[6] && !adj[3] && adj[5]) {
		pushRegion(vec4(0, 4, 4, 8), vec4(1.0, 0, 0.5, 0.5), 1.f);
	}
}