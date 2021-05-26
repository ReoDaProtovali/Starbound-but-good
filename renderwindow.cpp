
#include "renderwindow.hpp";

RenderWindow::RenderWindow(const char* p_title, int p_w, int p_h)
	:window(NULL), renderer(NULL)
{

	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, p_w, p_h, SDL_WINDOW_OPENGL);
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

void RenderWindow::render(Entity& p_entity) {
	SDL_Rect src;
	src.x = p_entity.getCurrentFrame().x;
	src.y = p_entity.getCurrentFrame().y;
	src.w = p_entity.getCurrentFrame().w;
	src.h = p_entity.getCurrentFrame().h;

	SDL_Rect dst;
	dst.x = p_entity.getX();
	dst.y = p_entity.getY();
	dst.w = p_entity.getCurrentFrame().w * 4;
	dst.h = p_entity.getCurrentFrame().h * 4;

	SDL_RenderCopy(renderer, p_entity.getTex(), &src, &dst);
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