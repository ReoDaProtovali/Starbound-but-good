
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
void GameWindow::drawChunk(WorldChunk& p_chunk) {
	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = 8;
	src.h = 8;
	//std::cout << "xy scale: " << cam.xyscale.x << " " << cam.xyscale.y << " pos: " << cam.pos.x << " " << cam.pos.y << " frame:" << cam.frame.x << " " << cam.frame.y << " " << cam.frame.w << " " << cam.frame.h << std::endl;
	Tile** tiles = p_chunk.getTiles();
	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			SDL_Rect dst;
			dst.x = std::floorf((p_chunk.worldPos.x * 128 + x + cam.pos.x) * cam.xyscale.x) - cam.frame.x;
			dst.y = std::floorf((p_chunk.worldPos.y * 128 + y + cam.pos.y) * cam.xyscale.y) - cam.frame.y;
			dst.w = std::ceilf(cam.xyscale.x);
			dst.h = std::ceilf(cam.xyscale.y);
			
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