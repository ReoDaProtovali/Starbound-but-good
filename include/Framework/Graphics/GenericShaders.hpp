#pragma once
#include "Shader.hpp"
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
	std::shared_ptr<Shader> imageShader;

private:
	GenericShaders();
	void init();
};