#pragma once
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include "Camera.hpp"
#include "Rect.hpp"

namespace SBBBDebugDraw {
	void drawBoxImmediate(float p_x, float p_y, float p_w, float p_h, glm::vec3 p_col, DrawSurface& p_surface, Camera& p_camera);
	void drawLineImmediate(float px1, float py1, float px2, float py2, glm::vec3 p_col, DrawSurface& p_surface, Camera& p_camera);
}