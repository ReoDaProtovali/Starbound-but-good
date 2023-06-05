#pragma once
#include <vector>
#include "Framework/Graphics/DrawSurface.hpp"
#include "util/Rect.hpp"
struct GUIEvent {
	KeyEvent key;
	MouseEvent mouse;
};
// A single element of a GUI in its most basic form. A widget can hold other widgets, and has a few virual methods.
class Widget {
public:
	virtual void draw(DrawSurface& p_target, DrawStates& p_states) = 0;
	// default behaviour, recursively update children
	virtual void onUpdate(GUIEvent e);
	void addChild(Widget* p_child);
	Rect queryScreenRect(Rect p_childRect);
protected:
	Rect localBounds;
	Rect screenBounds;
	std::vector<Widget*> m_children;
	Widget* m_parent = nullptr;
};