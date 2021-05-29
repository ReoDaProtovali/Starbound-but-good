
#include "Renderwindow.hpp";

RenderWindow::RenderWindow(const char* p_title, int p_w, int p_h)
	:window(NULL), renderer(NULL)
{

	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, p_w, p_h, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
	}

}

SDL_Texture* RenderWindow::loadTexture(const char* p_filepath) {
	SDL_Texture* texture = NULL;
	texture = IMG_LoadTexture(renderer, p_filepath);

	if (texture == NULL) {
		std::cout << "Failed to load texture: " << SDL_GetError() << std::endl;
		return NULL;
	}
	return texture;
}

void RenderWindow::renderEntity(Entity& p_entity) {
	SDL_Rect src = p_entity.getSrcRect();

	SDL_Rect dst;
	dst.x = (int)(p_entity.getPos().x);
	dst.y = (int)(p_entity.getPos().y);
	dst.w = src.w * 4;
	dst.h = src.h * 4;

	SDL_RenderCopy(renderer, p_entity.getTex(), &src, &dst);
}
void RenderWindow::drawChunk(WorldChunk& p_chunk) {
	int scale = 4;
	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = 8;
	src.h = 8;

	//for 
	//SDL_Rect dst;
	//dst.x = p_entity.getPos().x;
	//dst.y = p_entity.getPos().y;
	//dst.w = src.w * scale;
	//dst.h = src.h * scale;
	
	//SDL_RenderCopy(renderer, tex, &src, &dst);
}
void RenderWindow::display() {
	SDL_RenderPresent(renderer);
}
void RenderWindow::clear() {
	SDL_RenderClear(renderer);
}
void RenderWindow::cleanUp() {
	SDL_DestroyWindow(window);
}

int RenderWindow::getRefreshRate() {
	int displayIndex = SDL_GetWindowDisplayIndex(window);
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(displayIndex, 0, &mode);
	return mode.refresh_rate;
}