#include "SpriteSheet.hpp"

void SpriteSheet::setMode(SheetMode m_mode) {
	m_sheetMode = m_mode;
}

void SpriteSheet::nextFrame() {
	if (m_sheetMode == SheetMode::BASIC) {
		if (m_currentSprite + 1 > m_spriteCount) {
			throw std::length_error("Tried to advance to next sprite past the end of the tilesheet.");
		}
		else {
			m_currentSprite++;
		}
	}
	else if (m_sheetMode == SheetMode::ANIMATION_ROWS) {
		if (m_currentSprite + 1 >= m_frameCount) {
			m_currentSprite = 0;
		}
		else {
			m_currentSprite++;
		}
	}
}

glm::vec2 SpriteSheet::getTexCoords(Corner corner)
{
	glm::ivec2 spritePos = SpriteSheet::indexToPos(m_currentSprite);
	
	float x, y;
	switch (corner) {
	case Corner::TOP_LEFT:
		x = (float)spritePos.x / (float)m_texture.width;
		y = (float)spritePos.y / (float)m_texture.height;
		return glm::vec2(x, y);
	case Corner::TOP_RIGHT:
		x = ((float)spritePos.x + m_spriteDimensions.x) / (float)m_texture.width;
		y = ((float)spritePos.y) / (float)m_texture.height;
		return glm::vec2(x, y);
	case Corner::BOTTOM_LEFT:
		x = ((float)spritePos.x) / (float)m_texture.width;
		y = ((float)spritePos.y + m_spriteDimensions.y) / (float)m_texture.height;
		return glm::vec2(x, y);
	case Corner::BOTTOM_RIGHT:
		x = ((float)spritePos.x + m_spriteDimensions.x) / (float)m_texture.width;
		y = ((float)spritePos.y + m_spriteDimensions.y) / (float)m_texture.height;
		return glm::vec2(x, y);
	}
	return glm::vec2(-1.0f, -1.0f);
}

glm::ivec2 SpriteSheet::indexToPos(uint16_t p_spriteIndex) {
	if (m_sheetMode == SheetMode::BASIC) { // Basic mode, index starts at top left, and goes from top to bottom, left to right
		return glm::ivec2(
			(p_spriteIndex % (m_texture.width / m_spriteDimensions.x)) * m_spriteDimensions.x, // x pos
			(p_spriteIndex / m_spriteDimensions.y) * m_spriteDimensions.y); // y pos
	}
	else if (m_sheetMode == SheetMode::ANIMATION_ROWS) {
		return glm::ivec2(
			(p_spriteIndex % (m_texture.width / m_spriteDimensions.x)) * m_spriteDimensions.x, // x pos
			m_currentAnimation * m_spriteDimensions.y); // y pos
	}
	return glm::vec2(-1.0f, -1.0f);
}

void SpriteSheet::setCurrentSprite(uint16_t p_spriteIndex) {
	if (p_spriteIndex >= m_spriteCount) {
		throw std::out_of_range("Sprite requested is past the set sprite count.");
	}
	else {
		m_currentSprite = p_spriteIndex;
	}
}

void SpriteSheet::setSpriteDimensions(glm::ivec2 p_dim)
{
	m_spriteDimensions = p_dim;
}

void SpriteSheet::setAnimation(uint16_t p_animationID) {
	if (p_animationID >= m_animationCount) {
		throw std::out_of_range("Animation requested is past the set animation count.");
	}
	else {
		m_currentAnimation = p_animationID;
	}
}