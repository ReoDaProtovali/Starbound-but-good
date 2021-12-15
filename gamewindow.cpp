
#include "GameWindow.hpp";

GameWindow::GameWindow(const char* p_title, int p_w, int p_h)
	:window(NULL), renderer(NULL)
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	width = p_w;
	height = p_h;

	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, p_w, p_h, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	glContext = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, glContext);

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.0, 0.0, 0.0);
	glViewport(0, 0, width, height);

	glewExperimental = GL_TRUE;
	auto init_res = glewInit();
	if (init_res != GLEW_OK)
	{
		std::cout << glewGetErrorString(glewInit()) << std::endl;
	}

	cam = Camera(Vector2f(), Vector2i(100, 100));
}

void GameWindow::renderEntity(Entity& p_entity) {
	SDL_Rect src = p_entity.getSrcRect();

	SDL_Rect dst;
	dst.x = (int)(p_entity.getPos().x);
	dst.y = (int)(p_entity.getPos().y);
	dst.w = src.w * 4;
	dst.h = src.h * 4;

	SDL_RenderCopy(renderer, p_entity.getTex(), &src, &dst);
}
void GameWindow::drawChunk(WorldChunk& p_chunk) { // outdated
	//SDL_Rect src;
	//src.x = 0;
	//src.y = 0;
	//src.w = 8;
	//src.h = 8;
	////std::cout << "xy scale: " << cam.xyscale.x << " " << cam.xyscale.y << " pos: " << cam.pos.x << " " << cam.pos.y << " frame:" << cam.frame.x << " " << cam.frame.y << " " << cam.frame.w << " " << cam.frame.h << std::endl;
	//Tile** tiles = p_chunk.getTiles();
	//for (int y = 0; y < 128; y++) {
	//	for (int x = 0; x < 128; x++) {
	//		SDL_Rect dst;
	//		dst.x = std::floorf((p_chunk.worldPos.x * 128 + x + cam.pos.x) * cam.xyscale.x) - cam.frame.x;
	//		dst.y = std::floorf((p_chunk.worldPos.y * 128 + y + cam.pos.y) * cam.xyscale.y) - cam.frame.y;
	//		dst.w = std::ceilf(cam.xyscale.x);
	//		dst.h = std::ceilf(cam.xyscale.y);

	//		SDL_RenderCopy(renderer, res.getTex(tiles[y][x].tileID), &src, &dst);
	//	}
	//}

	//for 
	//SDL_Rect dst;
	//dst.x = p_entity.getPos().x;
	//dst.y = p_entity.getPos().y;
	//dst.w = src.w * scale;
	//dst.h = src.h * scale;

	//
}
void GameWindow::updateCamera() {
	cam.testUpdate(inpHandler, window);
}
void GameWindow::display() {
	SDL_RenderPresent(renderer);
}
void GameWindow::clear() {
	SDL_RenderClear(renderer);
}
void GameWindow::cleanUp() {
	SDL_DestroyWindow(window);
}

int GameWindow::getRefreshRate() {
	int displayIndex = SDL_GetWindowDisplayIndex(window);
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(displayIndex, 0, &mode);
	return mode.refresh_rate;
}