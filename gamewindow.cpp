#include "GameWindow.hpp"
#include "World.hpp"


GameWindow::GameWindow(const char* p_title, int p_w, int p_h)
	:window(NULL)
{

	width = p_w;
	height = p_h;

	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	GameWindow::initGL();

}
GameWindow::GameWindow(const char* p_title)
	:window(NULL)
{
	unsigned int defaultWidth = 1280;
	unsigned int defaultHeight = 720;

	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, defaultWidth, defaultHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	SDL_DisplayMode dm;

	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
	}

	width = defaultWidth;
	height = defaultHeight;

	//SDL_SetWindowSize(window, width / 2, height / 2);
	GameWindow::initGL();

}

void GameWindow::initGL() {
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); // Bit depth of 8 bytes
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // OpenGL 3.2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	glEnable(GL_BLEND); // Transparency blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glContext = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, glContext); // Attach OpenGL context to the window

	glewExperimental = GL_TRUE;
	auto init_res = glewInit();
	if (init_res != GLEW_OK)
	{
		std::cout << glewGetErrorString(glewInit()) << std::endl;
	}

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
	glViewport(0, 0, width, height);
}

void GameWindow::cleanUp() {
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
}
void GameWindow::bindAsRenderTarget() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 0 is equivalent to the main frame buffer, claimed by the SDL window.
	glDrawBuffer(GL_BACK); // The back of the double buffer, to be swapped in after rendering is finished with a call to SDL_GL_SwapWindow
	glViewport(0, 0, width, height); // Set the proper width and height for the viewport.
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