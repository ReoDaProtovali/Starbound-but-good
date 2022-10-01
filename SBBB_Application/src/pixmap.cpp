#include "Framework/Graphics/Pixmap.hpp"

Pixmap::Pixmap()
{
}

Pixmap::Pixmap(unsigned int p_width, unsigned int p_height) :
	width(p_width),
	height(p_height),
	pixels(p_width, p_height, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
{
}

glm::vec4 Pixmap::getPixel(unsigned int p_x, unsigned int p_y)
{
	if ((p_x < 0) || (p_x > width - 1) || (p_y < 0) || (p_y > height - 1)) {
		return glm::vec4();
	}
	return pixels(p_x, p_y);
}
void Pixmap::setPixel(unsigned int p_x, unsigned int p_y, glm::vec4 p_color)
{
	if ((p_x < 0) || (p_x > width - 1) || (p_y < 0) || (p_y > height - 1)) {
		return;
	}
	pixels(p_x, p_y) = p_color;
}
void Pixmap::clear() {
	pixels.fill(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}
void Pixmap::fill(glm::vec4 p_color)
{
	pixels.fill(p_color);
}
glm::vec4* Pixmap::getData() {
	return pixels.getData().data();
}
void Pixmap::logPixmap() {
	for (unsigned int i = 0; i < height; i++) {
		for (unsigned int j = 0; j < width; j++) {
			std::cout << pixels(j, i).r;
			std::cout << " ";
			std::cout << pixels(j, i).g;
			std::cout << " ";
			std::cout << pixels(j, i).b;
			std::cout << " ";
			std::cout << pixels(j, i).a;
			std::cout << ", ";
		}
		std::cout << std::endl;
	}
}