#pragma once
#include "Framework/Graphics/Shader.hpp"
#include <memory>

// I need to wrap all the generic shaders in a class so I can instantiate it after opengl initilizes and re-use them in various places without recompiling.
class GenericShaders {
public:
	GenericShaders(const GenericShaders&) = delete;

	static GenericShaders& Get() {
		static GenericShaders instance;
		return instance;
	}

	// Basic Image Shader
	// Very simple, draws the texture bound to unit 0 based on the texuture coordinates provided.
	// Attributes to use:
	// vec3 position
	// vec2 texcoord
	// Uniforms:
	// imageTexture: 0
	Shader imageShader;
	// Basic Solid Color Shader
	// Fills fragments with a flat color
	// Attributes to use:
	// vec3 position
	// vec2 texcoord (ignored, but there)
	// Uniforms:
	// vec3 color
	Shader solidColorShader;

	// for funsies
	// attribs:
	// vec3 pos
	// vec2 texCoord
	// uniform is time at location 1
	Shader fancyShader;
private:
	GenericShaders();
	void init();
};