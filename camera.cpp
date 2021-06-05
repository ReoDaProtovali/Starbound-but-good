#include "Camera.h"
void Camera::testUpdate(InputHandler& p_inph, SDL_Window* p_window) {
	SDL_GetWindowSize(p_window, &frame.w, &frame.h);
	int speed = 3;
	if (p_inph.testKey(SDLK_q)) {
		scale *= 0.96f;
	}
	if (p_inph.testKey(SDLK_e)) {
		scale *= 1.04f;
	}
	if (p_inph.testKey(SDLK_w)) {
		frame.y += speed;
	}
	if (p_inph.testKey(SDLK_a)) {
		frame.x += speed;
	}
	if (p_inph.testKey(SDLK_s)) {
		frame.y -= speed;
	}
	if (p_inph.testKey(SDLK_d)) {
		frame.x -= speed;
	}
}