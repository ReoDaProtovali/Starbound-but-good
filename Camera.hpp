#pragma once

#include "glm/glm.hpp"
#include <gtc/matrix_transform.hpp>
#include <algorithm>
struct Camera
{
	Camera() {
		pos = glm::vec3(0.0f, 0.0f, 1.0f); // starts 1 off the z axis by default
		target = glm::vec3(0.0f, 0.0f, 0.0f); // temporarily set the origin as the target to calculate the correct right and up
		glm::vec3 direction = glm::normalize(pos - target);
		right = glm::normalize(glm::cross(upGuide, direction));
		up = glm::cross(direction, right);
		forward = glm::cross(right, up); // forward calculate normalized forward facing vector
		scale = 20.0f; // default scaling of 20 tiles
		lookForwards();
	};
	Camera(glm::vec3 p_pos) {
		pos = p_pos; // starts 1 off the z axis by default
		target = glm::vec3(0.0f, 0.0f, 0.0f); // temporarily set the origin as the target to calculate the correct right and up
		glm::vec3 direction = glm::normalize(pos - target);
		right = glm::normalize(glm::cross(upGuide, direction));
		up = glm::cross(direction, right);
		forward = glm::cross(right, up); // forward calculate normalized forward facing vector
		scale = 20.0f; // default scaling of 20 tiles
		lookForwards();
	};

	void lookAt(glm::vec3 p_target) {
		view = glm::lookAt(pos, p_target, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	void lookForwards() {
		target = pos - forward; // look forwards
		view = glm::lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	void setDimensions(float p_aspectRatio) {
		dimensions.x = 1;
		dimensions.y = 1.0f / p_aspectRatio;
	}
	glm::mat4 getTransformMat4(float p_screenWidth, float p_screenHeight, float p_windowWidth, float p_windowHeight) {
		float screenScaling = 1.0;
		if (p_windowWidth < p_windowHeight) {
			screenScaling = (p_screenHeight/p_windowHeight) * (p_screenWidth / p_screenHeight);
		}
		else {
			screenScaling = (p_screenWidth/p_windowWidth);
		}
		if (!perspective) {
			proj = glm::ortho(
				0.0f,
				(dimensions.x) * scale * screenScaling,
				0.0f,
				(dimensions.y) * scale * screenScaling,
				0.1f, 1000.0f);
		}
		else {
			proj = glm::perspective(glm::radians(45.0f), p_screenWidth / p_screenHeight, 0.1f, 1000.0f);
		}
		if (!manualView) {
			lookForwards(); // default behavior, can be overriden by other view setting functions
		}
		glm::vec2 windowDims;
		if (p_windowHeight < p_windowWidth) {
			windowDims = glm::vec2(1.0f, p_windowHeight / p_windowWidth);
		}
		else {
			windowDims = glm::vec2(p_windowWidth / p_windowHeight, 1.0f);
		}
		glm::vec2 screenCenterPos = glm::vec2(windowDims / 2.0f) * scale;
		view = glm::translate(view, glm::vec3(screenCenterPos, 0.0f));
		return proj * view;
	}
	glm::vec2 getFramePos() {
		return glm::vec2(pos.x - (dimensions.x / 2.0f) * scale, pos.y + (dimensions.y / 2.0f) * scale);
	}
	//void setFrame(glm::vec2 ) {
	//	dimensions = p_gw.
	//}
	void setTileScale(int p_verticalTiles) {
		scale = p_verticalTiles;
	}
	void setGlobalPos(glm::vec2 p_globalPos) { // global pos is in the unit of tiles
		glm::vec2 pos_yInv = glm::vec2(p_globalPos.x, -p_globalPos.y);
		pos = glm::vec3(pos_yInv, pos.z);
	}
	void setGlobalPos(float p_globalX, float p_globalY) { // global pos is in the unit of tiles
		pos = glm::vec3(p_globalX, -p_globalY, pos.z);
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
	glm::mat4 proj;
	glm::mat4 view;
	glm::vec2 dimensions;
	float scale;
	bool manualView = false;
	bool perspective = false;

	float zRotation = 0;

};

