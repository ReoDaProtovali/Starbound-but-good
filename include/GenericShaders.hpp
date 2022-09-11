#pragma once
#include "Framework/Graphics/Shader.hpp"

// I don't know how to namespace this without instantiating a class, because each shader needs its uniforms set, and you need a function scope to do that.
// I'll settle for using the constructor of a struct to accomplish that.
// It's kind of bad practice, because the class memebers can be modified externally. I'll have to find a better way at some point, but I'm not certain how.
struct GenericShaders {
	GenericShaders() {
	}
	/**
	* A generic shader for drawing textures.
	* Vertex Attributes:
	* - aPos: A vec3 position.
	* - aTexCoord: vec2 UV coordinates.
	* Uniforms:
	* - sampler2d imageTexture; Bound to texure unit 0.
	*/
	Shader genImageShader() {
		Shader ImageShader = Shader("./src/Shaders/ImageVS.glsl", "./src/Shaders/ImageFS.glsl");
		ImageShader.setTexUniform("imageTexture", 0); // Texture unit 0
		return Shader(ImageShader);
	}
};
