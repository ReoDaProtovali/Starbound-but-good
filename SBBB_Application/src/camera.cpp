#include "Camera.hpp"
#include <iostream>

Camera::Camera() :
	pos(0.0f, 0.0f, 1.0f), // starts 1 off the z axis by default
	m_target(0.0f, 0.0f, 0.0f), // temporarily set the origin as the target to calculate the correct right and up
	tileScale(20.f)
{
	glm::vec3 direction = glm::normalize(pos - m_target);
	Camera::m_right = glm::normalize(glm::cross(m_upGuide, direction));
	Camera::m_up = glm::cross(direction, m_right);
	Camera::m_forward = glm::cross(m_right, m_up); // forward calculate normalized forward facing vector
	lookForwards();
	updateFrame();

}

Camera::Camera(glm::vec3 p_pos) :

	pos(p_pos),
	m_target(0.f, 0.f, 0.f), // temporarily set the origin as the target to calculate the correct right and up
	tileScale(20.f)
{
	glm::vec3 direction = glm::normalize(pos - m_target);
	Camera::m_right = glm::normalize(glm::cross(m_upGuide, direction));
	Camera::m_up = glm::cross(direction, m_right);
	Camera::m_forward = glm::cross(m_right, m_up); // forward calculate normalized forward facing vector
	lookForwards();
	updateFrame();
};

void Camera::lookAt(glm::vec3 p_target)
{
	Camera::view = glm::lookAt(pos, p_target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::lookForwards()
{
	Camera::m_target = pos - m_forward; // look forwards
	Camera::view = glm::lookAt(pos, m_target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::setDimensions(uint32_t p_windowWidth, uint32_t p_windowHeight)
{
	Camera::m_dimensions.x = 1.0f;
	Camera::m_dimensions.y = 1.0f / (float(p_windowWidth) / float(p_windowHeight));
	m_pixelDimensions.x = (float)p_windowWidth;
	m_pixelDimensions.y = (float)p_windowHeight;
	updateFrame();
}

glm::mat4 Camera::getTransform()
{
	float screenScaling = 1.0;
	if (m_pixelDimensions.x < m_pixelDimensions.y)
	{
		screenScaling = m_pixelDimensions.x / m_pixelDimensions.y;
	}
	else
	{
		screenScaling = 1.0f;
	}
	if (!m_perspective)
	{
		m_proj = glm::ortho(
			0.0f,
			m_dimensions.x * tileScale * screenScaling,
			0.0f,
			m_dimensions.y * tileScale * screenScaling,
			0.1f, 1000.0f);
	}
	else
	{
		m_proj = glm::perspective(glm::radians(45.0f), m_pixelDimensions.x / m_pixelDimensions.y, 0.1f, 1000.0f);
	}
	if (!m_manualView)
	{
		lookForwards(); // default behavior, can be overriden by other view setting functions
	}

	glm::vec2 screenCenterPos = glm::vec2((m_dimensions.x * screenScaling) / 2.0f, (m_dimensions.y * screenScaling) / 2.0f) * tileScale;
	setFrame(
		pos.x - screenCenterPos.x,
		pos.y - screenCenterPos.y,
		screenCenterPos.x * 2.0f,
		screenCenterPos.y * 2.0f);
	view = glm::translate(view, glm::vec3(screenCenterPos, 0.0f));
	return m_proj * view;
}

void Camera::setFrame(float p_trX, float p_trY, float p_width, float p_height)
{
	Camera::m_frame = glm::vec4(p_trX, p_trY, p_trX + p_width, p_trY + p_height);
}

const glm::vec2 Camera::getFrameDimensions()
{
	return glm::vec2(m_frame.z - m_frame.x, m_frame.y - m_frame.w);
}

void Camera::updateFrame()
{
	float screenScaling = 1.0;
	if (m_pixelDimensions.x < m_pixelDimensions.y)
	{
		screenScaling = m_pixelDimensions.x / m_pixelDimensions.y;
	}
	else
	{
		screenScaling = 1.0f;
	}
	glm::vec2 screenCenterPos = glm::vec2((m_dimensions.x * screenScaling) / 2.0f, (m_dimensions.y * screenScaling) / 2.0f) * tileScale;
	setFrame(
		pos.x - screenCenterPos.x,
		pos.y - screenCenterPos.y,
		screenCenterPos.x * 2.0f,
		screenCenterPos.y * 2.0f);
}

void Camera::setTileScale(float p_tileScale)
{
	Camera::tileScale = p_tileScale;
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
	return m_frame;
}