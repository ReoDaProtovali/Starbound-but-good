#include "SpriteSheet.hpp"

void SpriteSheet::setMode(SheetMode mode) {
	sheetMode = mode;
}

void SpriteSheet::nextFrame() {
	if (sheetMode == SheetMode::TILESHEET) {
		if (currentSprite + 1 > spriteCount) {
			throw std::length_error("Tried to advance to next sprite past the end of the tilesheet.");
		}
		else {
			currentSprite++;
		}
	}
	else if (sheetMode == SheetMode::ANIMATION_ROWS) {
		if (currentSprite + 1 > frameCount) {
			currentSprite = 0;
		}
		else {
			currentSprite++;
		}
	}
}

Vector2f SpriteSheet::getTexCoords(unsigned short p_spriteIndex, Corner corner)
{
	Vector2i pixelPos = SpriteSheet::indexToPos(p_spriteIndex);
	if (corner == Corner::TOP_LEFT) {
		float x = (float)image.dimensions.x / (float)pixelPos.x;
		float y = (float)image.dimensions.y / (float)pixelPos.y;
		return Vector2f(x, y);
	}
	if (corner == Corner::TOP_RIGHT) {
		float x = (float)image.dimensions.x / ((float)pixelPos.x + spriteDimensions.x);
		float y = (float)image.dimensions.y / ((float)pixelPos.y);
		return Vector2f(x, y);
	}
	if (corner == Corner::BOTTOM_LEFT) {
		float x = (float)image.dimensions.x / ((float)pixelPos.x);
		float y = (float)image.dimensions.y / ((float)pixelPos.y + spriteDimensions.y);
		return Vector2f(x, y);
	}
	if (corner == Corner::BOTTOM_RIGHT) {
		float x = (float)image.dimensions.x / ((float)pixelPos.x + spriteDimensions.x);
		float y = (float)image.dimensions.y / ((float)pixelPos.y + spriteDimensions.y);
		return Vector2f(x, y);
	}
}

Vector2i SpriteSheet::indexToPos(unsigned short p_spriteIndex) {
	if (sheetMode == SheetMode::TILESHEET) {
		return Vector2i(p_spriteIndex % (image.dimensions.x / spriteDimensions.x), p_spriteIndex / spriteDimensions.y);
	}
}

void SpriteSheet::setCurrentSprite(unsigned short p_spriteIndex) {
	if (p_spriteIndex >= spriteCount) {
		throw std::out_of_range("Sprite requested is past the set sprite count.");
	}
	else {
		currentSprite = p_spriteIndex;
	}
}

void SpriteSheet::setSpriteDimensions(Vector2i p_dim)
{
	spriteDimensions = p_dim;
}

