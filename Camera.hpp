#pragma once

#include "glm/glm.hpp"
#include <gtc/matrix_transform.hpp>
#include <algorithm>

class Camera
{
	Camera();
	Camera(glm::vec3 p_pos);

	void lookAt(glm::vec3 p_target);

	void lookForwards();

	void setDimensions(float p_aspectRatio);

	glm::mat4 getTransformMat4(float p_screenWidth, float p_screenHeight, float p_windowWidth, float p_windowHeight);

	void setFrame(float p_trX, float p_trY, float p_width, float p_height);

	void updateFrame(float p_windowHeight, float p_windowWidth);

	void setTileScale(float p_tileScale);

	void setGlobalPos(glm::vec2 p_globalPos);

	void setGlobalPos(float p_globalX, float p_globalY);

	void enableManualView() { manualView = true; }
	void disableManualView() { manualView = false; }
	void enablePerspective() { perspective = true; }
	void disablePerspective() { perspective = false; }


	glm::vec3 target;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 forward;
	glm::mat4 proj;
	glm::mat4 view;
	glm::vec2 dimensions;
	glm::vec4 frame;
	const glm::vec3 upGuide = glm::vec3(0.0f, 1.0f, 0.0f); // y axis is up
	bool manualView = false;
	bool perspective = false;

	public:
	glm::vec3 pos;
	float tileScale;
	float zRotation = 0;

};

