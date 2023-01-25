#include "Framework/Graphics/Pixmap.hpp"

Pixmap::Pixmap()
{
}

Pixmap::Pixmap(uint32_t p_width, uint32_t p_height) :
	width(p_width),
	height(p_height),
	m_pixels(p_width, p_height)
{
}

void Pixmap::resize(uint32_t p_width, uint32_t p_height)
{
	m_pixels.resize(p_width, p_height);
	width = p_width;
	height = p_height;
}

glm::vec4 Pixmap::getPixel(uint32_t p_x, uint32_t p_y)
{
	if ((p_x < 0) || (p_x > width - 1) || (p_y < 0) || (p_y > height - 1)) {
		return glm::vec4();
	}
	return m_pixels(p_x, p_y);
}
void Pixmap::appendImage(unsigned char* p_data, size_t p_size)
{
	if (p_size < 4) return;
	if (p_size % 4 != 0)
		throw std::exception("Pixel component mismatch!");
	if ((p_size / 4) % width != 0)
		throw std::exception("Width mismatch!");
	// p_size is the number of byte components, and there is one byte component per float component
	glm::vec4* tmp = (glm::vec4*)malloc(p_size * sizeof(float));
	if (!tmp) {
		throw std::exception("Malloc failed! Texture not appended.");
		return;
	}
	memset(tmp, 0, p_size * sizeof(float));
	for (size_t i = 0; i < p_size / 4; i++) {
		tmp[i].r = (float)p_data[i * 4 + 0] / 255.f;
		tmp[i].g = (float)p_data[i * 4 + 1] / 255.f;
		tmp[i].b = (float)p_data[i * 4 + 2] / 255.f;
		tmp[i].a = (float)p_data[i * 4 + 3] / 255.f;
	}
	m_pixels.append(tmp, p_size / 4);
	height = m_pixels.height;
	free(tmp);
}
void Pixmap::setPixel(uint32_t p_x, uint32_t p_y, glm::vec4 p_color)
{
	if ((p_x < 0) || (p_x > width - 1) || (p_y < 0) || (p_y > height - 1)) {
		return;
	}
	m_pixels(p_x, p_y) = p_color;
}
void Pixmap::clear() {
	m_pixels.fill(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}
void Pixmap::fill(glm::vec4 p_color)
{
	m_pixels.fill(p_color);
}
glm::vec4* Pixmap::getData() {
	return m_pixels.getData().data();
}
void Pixmap::logPixmap() {
	for (uint32_t i = 0; i < height; i++) {
		for (uint32_t j = 0; j < width; j++) {
			std::cout << m_pixels(j, i).r;
			std::cout << " ";
			std::cout << m_pixels(j, i).g;
			std::cout << " ";
			std::cout << m_pixels(j, i).b;
			std::cout << " ";
			std::cout << m_pixels(j, i).a;
			std::cout << ", ";
		}
		std::cout << std::endl;
	}
}