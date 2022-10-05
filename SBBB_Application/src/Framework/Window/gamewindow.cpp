#include "Framework/Window/GameWindow.hpp"

GameWindow::GameWindow(const char* p_title, int p_w, int p_h)
	:window(NULL)
{
#ifdef LOADLOGGING_ENABLED
	std::cout << "Creating game window titled \"" << p_title << "\"" << std::endl;
#endif
	width = p_w;
	height = p_h;

	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	GameWindow::initGL();

	// init inherited class's variables
	glBuffer = 0; // Main buffer
	m_DrawBuffers[0] = GL_BACK; // Back of double buffer
	setViewport(0, 0, width, height);

}
GameWindow::GameWindow(const char* p_title)
	:window(NULL),
	width(1280),
	height(720)
{
	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	SDL_DisplayMode dm;

	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
	}


	int displayIndex = SDL_GetWindowDisplayIndex(window);
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(displayIndex, 0, &mode);

	screenWidth = mode.w;
	screenHeight = mode.h;

	GameWindow::initGL();

	// init inherited class's variables
	glBuffer = 0; // Main buffer
	m_DrawBuffers[0] = GL_BACK; // Back of double buffer
	setViewport(0, 0, width, height);
}

void GameWindow::initGL() {
	glContext = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, glContext); // Attach OpenGL context to the window
}

void GameWindow::cleanUp() {
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
}
void GameWindow::setVSync(bool p_enabled)
{
	SDL_GL_SetSwapInterval((int)p_enabled);
}
void GameWindow::displayNewFrame()
{
	SDL_GL_SwapWindow(window); // Swap the back of the double buffer with the front.
}
int GameWindow::getRefreshRate() {
	int displayIndex = SDL_GetWindowDisplayIndex(window);
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(displayIndex, 0, &mode);
	return mode.refresh_rate;
}