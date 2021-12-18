#include "Camera.hpp"
void Camera::testUpdate(InputHandler& p_inph, SDL_Window* p_window) {

	float speed = 0.5f;
	if (p_inph.testKey(SDLK_q)) {
		globalScale *= 0.96f;
	}
	if (p_inph.testKey(SDLK_e)) {
		globalScale *= 1.04f;
	}
	Camera::recalculateView(p_window);

	if (p_inph.testKey(SDLK_w)) {
		pos.y += speed;
	}
	if (p_inph.testKey(SDLK_a)) {
		pos.x += speed;
	}
	if (p_inph.testKey(SDLK_s)) {
		pos.y -= speed;
	}
	if (p_inph.testKey(SDLK_d)) {
		pos.x -= speed;
	}
}

