#pragma once
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include "Camera.hpp"
#include "Rect.hpp"

namespace DebugDraw {
	void drawBoxImmediate(float p_x, float p_y, float p_w, float p_h, glm::vec3 p_col, DrawSurface& p_surface, Camera& p_camera);
}