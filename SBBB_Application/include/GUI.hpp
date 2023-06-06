#pragma once
#include "Framework/Window/GameWindow.hpp"
#include "Framework/Graphics/DrawStates.hpp"
#include "Framework/Graphics/DrawSurface.hpp"

class GUI {
public:
	GUI(const GameWindow& p_window);
	void draw(DrawSurface& p_target);
private:
	DrawStates GUIDrawStates;
};