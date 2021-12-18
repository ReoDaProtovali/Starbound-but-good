#pragma once

#ifndef VERTEX_STRUCT
#define VERTEX_STRUCT

#include "glm.hpp"
#define vec3 glm::vec3
#define vec2 glm::vec2
struct Vertex {
	Vertex(vec3 p_pos, vec2 p_texCoord) : pos(p_pos), texCoord(p_texCoord) {}
	vec3 pos;
	vec2 texCoord;
};

#endif