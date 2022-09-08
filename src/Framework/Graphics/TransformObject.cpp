#include "Framework/Graphics/TransformObject.hpp"
#include <cmath>
#include <util/ext/glm/gtc/matrix_transform.hpp>


const float fwrapUnsigned(float x, float r) {
	float aR = std::fabs(r); // The wrapping point should always be positive or else weird things happen
	// Piecewise fmod transformation function
	return x < 0 ? (std::fmodf(x, aR) + r) : std::fmodf(x, aR);
}
// wraps float values to the range [-r, r] 
const float fwrapSigned(float x, float r) {
	// Just apply a transform to the unsigned function
	return 2.f * fwrapUnsigned(0.5f * (x + r), r) - r;
}

namespace fw {
	TransformObject::TransformObject() :
		origin(glm::vec2(0.f, 0.f)),
		position(glm::vec3(0.f, 0.f, 0.f)),
		rotation(0.f),
		scale(glm::vec2(1.f, 1.f)),
		transform(glm::mat4(1.f))
	{}
	void TransformObject::setOrigin(glm::vec2 p_origin)
	{
		origin = p_origin;
		transformOutOfDate = true;
	}
	void TransformObject::setPosition(glm::vec3 p_position)
	{
		position = p_position;
		transformOutOfDate = true;
	}
	void TransformObject::setRotation(float radians)
	{
		rotation = fwrapUnsigned(radians, 3.141592653589f * 2.f);
	}
	void TransformObject::setScale(glm::vec2 p_scaleFactors)
	{
		scale = p_scaleFactors;
	}
	const glm::vec2 TransformObject::getOrigin()
	{
		return origin;
	}
	const glm::vec3 TransformObject::getPosition()
	{
		return position;
	}
	const float TransformObject::getRotation()
	{
		return rotation;
	}
	const glm::vec2 TransformObject::getScale()
	{
		return scale;
	}
	glm::mat4 TransformObject::getTransform()
	{
		if (transformOutOfDate) {
			// Matrix multiplication is non-commutative, so it needs to be done in TRS order in this case. It's always the reverse order than you expect it to be.
	
			// Translate to world space.
			transform = glm::translate(glm::mat4(1.0f), position);
			// Rotate along z axis, because in most cases that's the axis the camera is looking through.
			transform = glm::rotate(transform, rotation, glm::vec3(0.f, 0.f, 1.f));
			// Do the scaling.
			transform = glm::scale(transform, glm::vec3(scale, 1.f));
			// Transform to the origin, *technically* done first
			transform = glm::translate(transform, glm::vec3(-origin, 0.f));

		}

		return transform;
	}
}