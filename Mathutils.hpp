#pragma once
#include <math.h>
struct Vector2f {
	float x, y;
	Vector2f() : x(0.0f), y(0.0f) {}
	Vector2f(float p_x, float p_y) : x(p_x), y(p_y) {}
	void multiplyf(float p_multplier) { 
		x *= p_multplier;
		y *= p_multplier;
	}
	void multiply2f(Vector2f& p_vec) { 
		x *= p_vec.x; 
		y *= p_vec.y; 
	}
	void directionalForce(float p_angle, float p_magnitude) {
		x += cosf(p_angle) * p_magnitude;
		y += sinf(p_angle) * p_magnitude;
	}
	float length() { 
		return sqrtf((x * x) + (y * y));
	}
	float average2f(Vector2f& p_vec) { 
		x += (x + p_vec.x) / 2.0f;
		y += (y + p_vec.y) / 2.0f; 
	}


};
struct Vector2i {
	int x, y;
	Vector2i() : x(0), y(0) {}
	Vector2i(int p_x, int p_y) : x(p_x), y(p_y) {}
};