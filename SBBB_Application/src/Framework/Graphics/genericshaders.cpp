#pragma once

#include "Framework/Graphics/GenericShaders.hpp"

GenericShaders::GenericShaders()
{
	init();
}

void GenericShaders::init() {

	imageShader = std::make_shared<Shader>(
		"./src/Shaders/ImageVS.glsl",
		"./src/Shaders/ImageFS.glsl",
		std::vector<Uniform>{
			Uniform("imageTexture", UniformTypes::TEX, 0)
	});

}