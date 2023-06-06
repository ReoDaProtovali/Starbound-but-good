#include "GUI.hpp"
#include "util/ext/glm/gtc/matrix_transform.hpp"
GUI::GUI(const GameWindow& p_window)
{
	glm::mat4 mat = glm::translate(glm::mat4(1.f), glm::vec3(-1.f, 1.f, 0.f)) * glm::scale(glm::mat4(1.f), glm::vec3(2.f, -2.f, 1.f));
	GUIDrawStates.setTransform(mat);
}

void GUI::draw(DrawSurface& p_target)
{
}
