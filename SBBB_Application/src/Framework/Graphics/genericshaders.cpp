#pragma once

#include "Framework/Graphics/GenericShaders.hpp"

GenericShaders::GenericShaders()
{
	init();
}

void GenericShaders::init() {

	imageShader = Shader(
		"./src/Shaders/ImageVS.glsl",
		"./src/Shaders/ImageFS.glsl"
);
	imageShader.setTexUniform(1, 0);

}