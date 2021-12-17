#pragma once
#include "MathUtils.hpp";
#include "GameConstants.h"
#include "Image.hpp"
#include <string>
#include <stdexcept>


class SpriteSheet
{
	public:
		SpriteSheet()
			: sheetMode(SheetMode::TILESHEET),
			image(Image()),
			currentSprite(NULL),
			spriteCount(NULL),
			currentAnimation(NULL),
			animationCount(NULL),
			frameCount(NULL) {};

		SpriteSheet(Image p_image, Vector2i p_spriteDimensions, unsigned short p_spriteCount)
			: sheetMode(SheetMode::TILESHEET),
			image(p_image),
			spriteDimensions(p_spriteDimensions),
			currentSprite(NULL), 
			spriteCount(p_spriteCount), 
			currentAnimation(NULL), 
			animationCount(NULL), 
			frameCount(1) {};


		void setImage(Image p_image) { image = p_image; };
		void setSpriteCount(unsigned short p_spriteCount) { spriteCount = p_spriteCount; };
		void setCurrentSprite(unsigned short p_spriteIndex);
		void setSpriteDimensions(Vector2i p_dim);
		void setMode(SheetMode mode);

		void nextFrame();
		Vector2i indexToPos(unsigned short p_spriteIndex);
		Vector2f getTexCoords(unsigned short p_spriteID, Corner corner);
		void setAnimation(unsigned short p_animationID);
	private:
		SheetMode sheetMode;
		Image image;
		unsigned short currentSprite;
		unsigned short spriteCount;

		unsigned short currentAnimation;
		unsigned short frameCount;

		unsigned short animationCount;


		Vector2i spriteDimensions;
};

