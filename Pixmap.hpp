#pragma once
#include "glm/glm.hpp"
#include <iostream>
#include "Array2D.hpp"

class Pixmap
{
public:
	Pixmap() { width = 0; height = 0; pixels = Array2D<glm::vec4>(); };
	Pixmap(int p_width, int p_height) {
		width = p_width;
		height = p_height;
		pixels = Array2D<glm::vec4>(p_width, p_height);
		pixels.fill(glm::vec4(0.0f, 0.0f, 0.5f, 1.0f));
	};
	int width;
	int height;
	glm::vec4 getPixel(int p_y, int p_x);
	void setPixel(int p_x, int p_y, glm::vec4 p_color);
	void clear();
	glm::vec4* getData();
	void logPixmap();
	

private:
	Array2D<glm::vec4> pixels;
};

