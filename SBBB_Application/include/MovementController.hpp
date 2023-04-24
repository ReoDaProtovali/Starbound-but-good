#pragma once
#include "glm/glm.hpp"

enum CardinalDirection : uint8_t {
	UP = 0b1000,
	LEFT = 0b0100,
	DOWN = 0b0010,
	RIGHT = 0b0001
};
enum {
	OFF,
	ON
};
// very simple class but reduces clutter
class MovementController {
public:
	// input any of the 4 cardinal directions, using the enum.
	virtual void dpadControl(CardinalDirection dir, bool setState = ON) {
		if (setState)
			m_dpadState |= dir;
		else
			m_dpadState &= ~dir;
	}

	virtual void stickControl(float x, float y) {
		m_dpadState = 0; // dpad control overridden, don't use
		m_vecState = glm::normalize(glm::vec2(x, y));
	}
	virtual glm::vec2 getMovementDir() { 
		glm::vec2 outDir{ 0 };
		if (m_dpadState) {
			if (m_dpadState & CardinalDirection::UP) outDir.y++;
			if (m_dpadState & CardinalDirection::DOWN) outDir.y--;
			if (m_dpadState & CardinalDirection::RIGHT) outDir.x++;
			if (m_dpadState & CardinalDirection::LEFT) outDir.x--;
			if (!holdInput) m_dpadState = 0.f;
			if (outDir.x == 0.f && outDir.y == 0.0f) return glm::vec2(0.f);
			return glm::normalize(outDir);
		}
		outDir = m_vecState;
		if (!holdInput) m_vecState = glm::vec2(0.f);
		return outDir; 
	};
	bool holdInput = false;

private:
	uint8_t m_dpadState = 0;
	glm::vec2 m_vecState{ 0 };
};