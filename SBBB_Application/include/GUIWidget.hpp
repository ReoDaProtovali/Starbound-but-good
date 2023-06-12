#pragma once
#include <vector>
#include "Framework/Graphics/DrawSurface.hpp"
#include "util/Rect.hpp"
#include <stdbool.h>
struct GUIEvent {
	MouseEvent mouse;
	KeyEvent key;
};
// A single element of a GUI in its most basic form. A widget can hold other widgets, and has a few virual methods.
class Widget {
public:
	Widget();
	Widget(std::string_view p_ID, Rect p_localBounds);
	void setID(std::string_view p_newID);

	// default behaviour, recursively draw children
	virtual void draw(DrawSurface& p_target, DrawStates& p_states);
	// default behaviour, recursively update children
	virtual void onUpdate(GUIEvent e);

	void addChild(Widget* p_child);

	Rect queryAbsoluteRect(Rect p_childRect);

	Widget* findChild(std::string_view p_childID);
	Widget* findChildRecursive(std::string_view p_childID);
	// only removes surface level children, returns false if not found.
	bool removeChild(std::string_view p_childID);
	bool removeChildRecursive(std::string_view p_childID);

protected:
	Rect localBounds;
	Rect absoluteBounds;

	std::string m_ID;
	std::vector<Widget*> m_children;
	Widget* m_parent = nullptr;
};