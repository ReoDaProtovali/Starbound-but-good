#include "SpriteSheet.hpp"

void SpriteSheet::setMode(SheetMode mode) {
	sheetMode = mode;
}

void SpriteSheet::nextFrame() {
	if (sheetMode == SheetMode::BASIC) {
		if (currentSprite + 1 > spriteCount) {
			throw std::length_error("Tried to advance to next sprite past the end of the tilesheet.");
		}
		else {
			currentSprite++;
		}
	}
	else if (sheetMode == SheetMode::ANIMATION_ROWS) {
		if (currentSprite + 1 >= frameCount) {
			currentSprite = 0;
		}
		else {
			currentSprite++;
		}
	}
}

glm::vec2 SpriteSheet::getTexCoords(Corner corner)
{
	glm::ivec2 spritePos = SpriteSheet::indexToPos(currentSprite);
	
	float x, y;
	switch (corner) {
	case Corner::TOP_LEFT:
		x = (float)spritePos.x / (float)texture.width;
		y = (float)spritePos.y / (float)texture.height;
		return glm::vec2(x, y);
	case Corner::TOP_RIGHT:
		x = ((float)spritePos.x + spriteDimensions.x) / (float)texture.width;
		y = ((float)spritePos.y) / (float)texture.height;
		return glm::vec2(x, y);
	case Corner::BOTTOM_LEFT:
		x = ((float)spritePos.x) / (float)texture.width;
		y = ((float)spritePos.y + spriteDimensions.y) / (float)texture.height;
		return glm::vec2(x, y);
	case Corner::BOTTOM_RIGHT:
		x = ((float)spritePos.x + spriteDimensions.x) / (float)texture.width;
		y = ((float)spritePos.y + spriteDimensions.y) / (float)texture.height;
		return glm::vec2(x, y);
	}
	return glm::vec2(-1.0f, -1.0f);
}

glm::ivec2 SpriteSheet::indexToPos(unsigned short p_spriteIndex) {
	if (sheetMode == SheetMode::BASIC) { // Basic mode, index starts at top left, and goes from top to bottom, left to right
		return glm::ivec2(
			(p_spriteIndex % (texture.width / spriteDimensions.x)) * spriteDimensions.x, // x pos
			(p_spriteIndex / spriteDimensions.y) * spriteDimensions.y); // y pos
	}
	else if (sheetMode == SheetMode::ANIMATION_ROWS) {
		return glm::ivec2(
			(p_spriteIndex % (texture.width / spriteDimensions.x)) * spriteDimensions.x, // x pos
			currentAnimation * spriteDimensions.y); // y pos
	}
	return glm::vec2(-1.0f, -1.0f);
}

void SpriteSheet::setCurrentSprite(unsigned short p_spriteIndex) {
	if (p_spriteIndex >= spriteCount) {
		throw std::out_of_range("Sprite requested is past the set sprite count.");
	}
	else {
		currentSprite = p_spriteIndex;
	}
}

void SpriteSheet::setSpriteDimensions(glm::ivec2 p_dim)
{
	spriteDimensions = p_dim;
}

void SpriteSheet::setAnimation(unsigned short p_animationID) {
	if (p_animationID >= animationCount) {
		throw std::out_of_range("Animation requested is past the set animation count.");
	}
	else {
		currentAnimation = p_animationID;
	}
}