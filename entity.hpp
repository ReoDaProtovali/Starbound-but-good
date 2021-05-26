#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

class Entity
{
public:
	Entity(float p_x, float p_y, SDL_Texture* p_tex);
	float getX();
	float getY();
	void setX(float p_x);
	void setY(float p_y);
	SDL_Texture* getTex();
	SDL_Rect getCurrentFrame();
private:
	float x;
	float y;
	SDL_Rect currentFrame;
	SDL_Texture* tex;
};

