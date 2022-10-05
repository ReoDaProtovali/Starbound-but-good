#pragma once
#include <string>
#include <GL/glew.h>
#include <util/ext/glm/mat4x4.hpp>

// shrimple dimple
// used for initilizing uniforms in the constructor of shaders

enum class UniformTypes {
	UNDEFINED,
	FLOAT,
	TEX,
	MAT4,
	INT,
	BOOL
};
struct Uniform {
	// separated constructors for the different types
	// the wall of text
	Uniform(std::string p_name, UniformTypes p_type, GLint p_value) : name(p_name), type(p_type), intValue(p_value) {};
	Uniform(std::string p_name, UniformTypes p_type, GLfloat p_value) : name(p_name), type(p_type), floatValue(p_value) {};
	Uniform(std::string p_name, UniformTypes p_type, bool p_value) : name(p_name), type(p_type), intValue((int)p_value) {};
	Uniform(std::string p_name, UniformTypes p_type, glm::mat4 p_value) : name(p_name), type(p_type), mat4Value(p_value) {};

	// god this is scuffed but it's the best way I can think of doing it because this class needs to be a fixed type because I need to be able to put multiple of different value types in the same array
	// the int value is shared by tex, bool, and int types because they are indistinguishable
	// Might make this a union later, but mat4 is substantially bigger so idk
	GLint intValue = -1;
	GLfloat floatValue = -1.f;
	glm::mat4 mat4Value = glm::mat4(0.f);

	const std::string name;

	// Type determines which internal type the shader should fetch. 
	UniformTypes type = UniformTypes::UNDEFINED;
};
