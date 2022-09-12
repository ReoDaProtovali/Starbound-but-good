#include "Camera.hpp"
#include <iostream>
Camera::Camera()
{
	pos = glm::vec3(0.0f, 0.0f, 1.0f);    // starts 1 off the z axis by default
	target = glm::vec3(0.0f, 0.0f, 0.0f); // temporarily set the origin as the target to calculate the correct right and up
	glm::vec3 direction = glm::normalize(pos - target);
	right = glm::normalize(glm::cross(upGuide, direction));
	up = glm::cross(direction, right);
	forward = glm::cross(right, up);                 // forward calculate normalized forward facing vector
	tileScale = 20.0f;                               // default scaling of 20 tiles
	frame = glm::vec4(-100.0, -100.0, 100.0, 100.0); // fixes a bug
	lookForwards();
}

Camera::Camera(glm::vec3 p_pos)
{
	pos = p_pos;
	target = glm::vec3(0.0f, 0.0f, 0.0f); // temporarily set the origin as the target to calculate the correct right and up
	glm::vec3 direction = glm::normalize(pos - target);
	right = glm::normalize(glm::cross(upGuide, direction));
	up = glm::cross(direction, right);
	forward = glm::cross(right, up); // forward calculate normalized forward facing vector
	tileScale = 20.0f;               // default scaling of 20 tiles

	lookForwards();
};

void Camera::lookAt(glm::vec3 p_target)
{
	view = glm::lookAt(pos, p_target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::lookForwards()
{
	target = pos - forward; // look forwards
	view = glm::lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::setDimensions(float p_aspectRatio)
{
	dimensions.x = 1.0f;
	dimensions.y = 1.0f / p_aspectRatio;
}

glm::mat4 Camera::getTransform()
{
	float screenScaling = 1.0;
	if (pixelDimensions.x < pixelDimensions.y)
	{
		screenScaling = pixelDimensions.x / pixelDimensions.y;
	}
	else
	{
		screenScaling = 1.0f;
	}
	if (!perspective)
	{
		proj = glm::ortho(
			0.0f,
			dimensions.x * tileScale * screenScaling,
			0.0f,
			dimensions.y * tileScale * screenScaling,
			0.1f, 1000.0f);
	}
	else
	{
		proj = glm::perspective(glm::radians(45.0f), pixelDimensions.x / pixelDimensions.y, 0.1f, 1000.0f);
	}
	if (!manualView)
	{
		lookForwards(); // default behavior, can be overriden by other view setting functions
	}

	glm::vec2 screenCenterPos = glm::vec2((dimensions.x * screenScaling) / 2.0f, (dimensions.y * screenScaling) / 2.0f) * tileScale;
	setFrame(
		pos.x - screenCenterPos.x,
		pos.y - screenCenterPos.y,
		screenCenterPos.x * 2.0f,
		screenCenterPos.y * 2.0f);
	view = glm::translate(view, glm::vec3(screenCenterPos, 0.0f));
	return proj * view;
}

void Camera::setFrame(float p_trX, float p_trY, float p_width, float p_height)
{
	frame = glm::vec4(p_trX, p_trY, p_trX + p_width, p_trY + p_height);
}

void Camera::updateFrame()
{
	float screenScaling = 1.0;
	if (pixelDimensions.x < pixelDimensions.y)
	{
		screenScaling = pixelDimensions.x / pixelDimensions.y;
	}
	else
	{
		screenScaling = 1.0f;
	}
	glm::vec2 screenCenterPos = glm::vec2((dimensions.x * screenScaling) / 2.0f, (dimensions.y * screenScaling) / 2.0f) * tileScale;
	setFrame(
		pos.x - screenCenterPos.x,
		pos.y - screenCenterPos.y,
		screenCenterPos.x * 2.0f,
		screenCenterPos.y * 2.0f);
}

void Camera::setTileScale(float p_tileScale)
{
	tileScale = p_tileScale;
}

void Camera::setGlobalPos(glm::vec2 p_globalPos)
{ // global pos is in the unit of tiles
	glm::vec2 pos_yInv = glm::vec2(p_globalPos.x, -p_globalPos.y);
	pos = glm::vec3(pos_yInv, pos.z);
}

void Camera::setGlobalPos(float p_globalX, float p_globalY)
{ // global pos is in the unit of tiles
	pos = glm::vec3(p_globalX, -p_globalY, pos.z);
}

const glm::vec4 Camera::getFrame() {
	return frame;
}