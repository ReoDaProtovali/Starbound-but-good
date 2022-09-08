#pragma once

#include <util/ext/glm/vec2.hpp>
#include <util/ext/glm/vec3.hpp>
#include <util/ext/glm/vec4.hpp>
#include <util/ext/glm/mat4x4.hpp>

namespace fw {
	// A flat 2d object with z layering.
	class TransformObject {
	public:
		TransformObject();

		// Sets the origin to be used. Affects rotation, position, etc
		void setOrigin(glm::vec2 p_origin);

		// 3d position
		// Overwrites old position
		void setPosition(glm::vec3 p_position);

		// 2d Rotation along the z axis
		// Overwrites old rotation
		void setRotation(float radians);

		// xy scale
		void setScale(glm::vec2 p_scaleFactors);

		const glm::vec2 getOrigin();

		const glm::vec3 getPosition();

		const float getRotation();

		const glm::vec2 getScale();

		glm::mat4 getTransform();

	private:
		glm::vec2 origin; 
		glm::vec3 position; // Decided to give it a z axis, as this is handy for certain effects and layering.
		float rotation; // Rotation does not affect the z axis. I can make a 3d transform object if needed later. Strictly radians.
		glm::vec2 scale; // xy
		glm::mat4 transform;
		bool transformOutOfDate = true;
	};
}