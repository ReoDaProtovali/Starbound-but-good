#pragma once

#ifndef VERTEX_STRUCT
#define VERTEX_STRUCT

#include "glm.hpp"

struct Vertex {
	Vertex(glm::vec3 p_pos, glm::vec2 p_texCoord) : pos(p_pos), texCoord(p_texCoord) {}
	glm::vec3 pos;
	glm::vec2 texCoord;
};

#endif