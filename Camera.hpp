#pragma once

#include "glm/glm.hpp"
#include <gtc/matrix_transform.hpp>

struct Camera
{
	Camera() {
		pos = glm::vec3(0.0f, 0.0f, 1.0f); // starts 1 off the z axis by default
		target = glm::vec3(0.0f, 0.0f, 0.0f); // temporarily set the origin as the target to calculate the correct right and up
		glm::vec3 direction = glm::normalize(pos - target);
		right = glm::normalize(glm::cross(upGuide, direction));
		up = glm::cross(direction, right);
		forward = glm::cross(right, up); // forward calculate normalized forward facing vector
		scale = glm::vec2(1 / 8.0f, 1 / 8.0f); // default scaling of tiles being the same size as their textures
		lookForwards();
	};

	void lookAt(glm::vec3 p_target) {
		view = glm::lookAt(pos, p_target, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	void lookForwards() {
		target = pos - forward; // look forwards
		view = glm::lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::mat4 getTransformMat4(float p_camWidth, float p_camHeight) {
		float cw_scaled = p_camWidth * scale.x;
		float ch_scaled = p_camHeight * scale.y;
		glm::mat4 proj;
		if (!perspective) {
			proj = glm::ortho(-cw_scaled / 2, cw_scaled / 2, -ch_scaled / 2, ch_scaled / 2, 0.1f, 1000.0f);
		}
		else {
			proj = glm::perspective(glm::radians(45.0f), p_camWidth / p_camHeight, 0.1f, 1000.0f);
		}
		if (!manualView) {
			lookForwards(); // default behavior, can be overriden by other view setting functions
		}
		return proj * view;
	}
	void setTileScale(int p_tileWidth) {
		scale = glm::vec2(1.0f / p_tileWidth, 1.0f / p_tileWidth);
	}
	void enableManualView() { manualView = true; }
	void disableManualView() { manualView = false; }
	void enablePerspective() { perspective = true; }
	void disablePerspective() { perspective = false; }

	const glm::vec3 upGuide = glm::vec3(0.0f, 1.0f, 0.0f); // y axis is up
	glm::vec3 pos;
	glm::vec3 target;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 forward;
	glm::mat4 view;
	glm::vec2 scale;
	bool manualView = false;
	bool perspective = false;

	float zRotation = 0;

};

