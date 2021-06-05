
#include "GameWindow.hpp";

GameWindow::GameWindow(const char* p_title, int p_w, int p_h)
	:window(NULL), renderer(NULL)
{

	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, p_w, p_h, SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
	}
	res = ResourceLoader(renderer);
	SDL_Rect defaultCameraPos;
	defaultCameraPos.x = 0;
	defaultCameraPos.y = 0;
	defaultCameraPos.w = p_w;
	defaultCameraPos.h = p_h;
	cam = Camera(defaultCameraPos, 1);
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
void GameWindow::drawChunk(WorldChunk& p_chunk) {
	int scale = cam.scale;
	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = 8;
	src.h = 8;
	Tile** tiles = p_chunk.getTiles();
	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			SDL_Rect dst;
			dst.x = std::floorf((x * 16 + cam.frame.x) * cam.scale);
			dst.y = std::floorf((y * 16 + cam.frame.y) * cam.scale);
			dst.w = std::ceilf(16 * cam.scale);
			dst.h = std::ceilf(16 * cam.scale);
			SDL_RenderCopy(renderer, res.getTex(tiles[y][x].tileID), &src, &dst);
		}
	}

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