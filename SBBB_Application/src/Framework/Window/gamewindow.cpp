#include "Framework/Window/GameWindow.hpp"

GameWindow::GameWindow(const char* p_title, int p_w, int p_h)
	:m_window(NULL)
{
#ifdef LOADLOGGING_ENABLED
	std::cout << "Creating game window titled \"" << p_title << "\"" << std::endl;
#endif
	windowWidth = p_w;
	windowHeight = p_h;

	m_window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (m_window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}

	int displayIndex = SDL_GetWindowDisplayIndex(m_window);
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(displayIndex, 0, &mode);

	screenWidth = mode.w;
	screenHeight = mode.h;

	GameWindow::initGL();

	// init inherited class's variables
	m_DrawBuffers[0] = GL_BACK; // Back of double buffer
	setViewport(0, 0, windowWidth, windowHeight);

}
GameWindow::GameWindow(const char* p_title)
	:m_window(NULL),
	windowWidth(1280),
	windowHeight(720)
{
	m_window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (m_window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	SDL_DisplayMode dm;

	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
	}


	int displayIndex = SDL_GetWindowDisplayIndex(m_window);
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(displayIndex, 0, &mode);

	screenWidth = mode.w;
	screenHeight = mode.h;

	GameWindow::initGL();

	// init inherited class's variables
	m_DrawBuffers[0] = GL_BACK; // Back of double buffer
	setViewport(0, 0, windowWidth, windowHeight);
}

void GameWindow::initGL() {
	m_glContext = SDL_GL_CreateContext(m_window);
	SDL_GL_MakeCurrent(m_window, m_glContext); // Attach OpenGL context to the window
}

void GameWindow::cleanUp() {
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
}
void GameWindow::setVSync(bool p_enabled)
{
	SDL_GL_SetSwapInterval((int)p_enabled);
}
void GameWindow::displayNewFrame()
{
	SDL_GL_SwapWindow(m_window); // Swap the back of the double buffer with the front.
}
int GameWindow::getRefreshRate() {
	int displayIndex = SDL_GetWindowDisplayIndex(m_window);
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(displayIndex, 0, &mode);
	return mode.refresh_rate;
}