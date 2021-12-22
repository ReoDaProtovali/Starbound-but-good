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

	glm::mat4 getTransformMat4(float p_aspectRatio, float p_screenWidth, float p_screenHeight, float p_gameWidth, float p_gameHeight) {
		float hUnit = p_screenHeight / p_gameHeight;
		float wUnit = p_screenWidth / p_gameWidth;
		float finalUnit;
		if (wUnit < hUnit) {
			finalUnit = wUnit;
		}
		else {
			finalUnit = hUnit;
		}
		float cw_scaled = scale * finalUnit;
		float ch_scaled = scale * finalUnit * p_aspectRatio;
		if (!perspective) {
			proj = glm::ortho(
				0.0f, // Left
				cw_scaled, // Right
				0.0f, // Bottom
				ch_scaled, // Top
				0.1f, 1000.0f);
		}
		else {
			proj = glm::perspective(glm::radians(45.0f), p_aspectRatio, 0.1f, 1000.0f);
		}
		if (!manualView) {
			lookForwards(); // default behavior, can be overriden by other view setting functions
		}
		return proj * view;
	}
	glm::vec2 getFramePos() {
		return glm::vec2(pos.x - dimensions.x / 2, pos.y + dimensions.y / 2);
	}
	//void setFrame(glm::vec2 ) {
	//	dimensions = p_gw.
	//}
	void setTileScale(int p_verticalTiles) {
		scale = p_verticalTiles;
	}
	//void multScaleCentered(float mult) {
	//	scale *= mult;
	//}
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

