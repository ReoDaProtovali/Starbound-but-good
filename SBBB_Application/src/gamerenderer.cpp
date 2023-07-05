#include "GameRenderer.hpp"
#include <sstream>
#include <iomanip>

GameRenderer::GameRenderer(GameWindow& p_window) :
	window(p_window)
{

	LOAD_LOG("GameRenderer instantiated...");
	cam = &defaultCam;
	screenFBO.getColorTexRef(0).setWrapping(GL_CLAMP_TO_EDGE);
}


void GameRenderer::setClearColor(glm::vec4 p_col)
{
	screenFBO.setClearColor(p_col);
}

void GameRenderer::clearScreen()
{
	screenFBO.clear();
}

void GameRenderer::setViewport(uint16_t p_w, uint16_t p_h)
{
	screenFBO.setDimensions(p_w, p_h);
	cam->setDimensions(p_w, p_h);
	cam->updateFrame();
}

void GameRenderer::setCam(Camera* p_cam)
{
	cam = p_cam;
}

