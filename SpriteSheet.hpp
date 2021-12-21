#pragma once
#include "glm/glm.hpp"
#include "GameConstants.hpp"
#include "Image.hpp"
#include <string>
#include <stdexcept>


class SpriteSheet
{
	public:
		SpriteSheet()
			: sheetMode(SheetMode::TILESHEET),
			image(Image()),
			spriteDimensions(glm::ivec2()),
			currentSprite(NULL),
			spriteCount(NULL),
			currentAnimation(NULL),
			animationCount(NULL),
			frameCount(NULL) {};

		SpriteSheet(Image p_image, glm::ivec2 p_spriteDimensions, unsigned short p_spriteCount)
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
		void setSpriteDimensions(glm::ivec2 p_dim);
		void setMode(SheetMode mode);

		void nextFrame();
		glm::ivec2 indexToPos(unsigned short p_spriteIndex);
		glm::vec2 getTexCoords(unsigned short p_spriteID, Corner corner);
		void setAnimation(unsigned short p_animationID);
		Image image;
		unsigned int glTexture = 0;
		bool hasglTexture = false;
		inline void setglTexture(unsigned int p_glTexID) { glTexture = p_glTexID; hasglTexture = true; };


	private:
		SheetMode sheetMode;
		unsigned short currentSprite;
		unsigned short spriteCount;

		unsigned short currentAnimation;
		unsigned short frameCount;

		unsigned short animationCount;


		glm::ivec2 spriteDimensions;
};

