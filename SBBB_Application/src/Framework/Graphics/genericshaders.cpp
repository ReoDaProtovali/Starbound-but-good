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

	solidColorShader = Shader(
		"./src/Shaders/SolidColorVS.glsl",
		"./src/Shaders/SolidColorFS.glsl"
	);
	solidColorShader.setVec3Uniform(1, glm::vec3(0.f));

	fancyShader = Shader(
		"./src/Shaders/ImageVS.glsl",
		"./src/Shaders/FancyFS.glsl"
	);
	fancyShader.setFloatUniform(1, 0.f);

}