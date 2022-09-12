#pragma once
#include "Shader.hpp"

// I need to wrap all the generic shaders in a class so I can instantiate it after opengl initilizes and re-use them in various places without recompiling.
struct GenericShaders {
	GenericShaders();
	void init();

	// Basic Image Shader
	// Very basic, draws the texture bound to unit 0 based on the texuture coordinates provided.
	// Attributes to use:
	// vec3 position
	// vec2 texcoord
	// Uniforms:
	// imageTexture: 0
	Shader imageShader;
};