#include "GUIWidget.hpp"

Widget::Widget()
{
}

Widget::Widget(std::string_view p_ID, Rect p_localBounds) :
	m_ID(p_ID), localBounds(p_localBounds)
{
}

void Widget::setID(std::string_view p_newID)
{
	m_ID = p_newID;
}

void Widget::draw(DrawSurface& p_target, DrawStates& p_states)
{
	for (auto child : m_children) {
		child->draw(p_target, p_states);
	}
}

void Widget::onUpdate(GUIEvent e)
{
	for (Widget* w : m_children) {
		w->onUpdate(e);
	}
}

void Widget::addChild(Widget* p_child)
{
	m_children.push_back(p_child);
	p_child->m_parent = this;
	p_child->absoluteBounds = p_child->queryAbsoluteRect(p_child->localBounds);
}

Rect Widget::queryAbsoluteRect(Rect p_childRect)
{
	if (m_parent == nullptr) return p_childRect;
	// untested coordinate escalation
	float newX = m_parent->localBounds.xy.x + m_parent->localBounds.wh.x * p_childRect.xy.x;
	float newY = m_parent->localBounds.xy.y + m_parent->localBounds.wh.y * p_childRect.xy.y;
	float newW = m_parent->localBounds.wh.x * p_childRect.wh.x;
	float newH = m_parent->localBounds.wh.y * p_childRect.wh.y;
	return m_parent->queryAbsoluteRect(Rect(newX, newY, newW, newH));
}

void Widget::setLocalBounds(Rect p_localBounds)
{
	m_absolute = false;
	m_usingScreenBounds = false;
	localBounds = p_localBounds;
	absoluteBounds = queryAbsoluteRect(localBounds);
}

void Widget::setAbsoluteBounds(Rect p_absoluteBounds)
{
	m_absolute = true;
	absoluteBounds = p_absoluteBounds;
}

void Widget::setScreenBounds(Rect p_screenBounds)
{
	m_absolute = true;
	m_usingScreenBounds = true;
	screenBounds = p_screenBounds;
}

void Widget::updateScreenBounds(float p_windowWidth, float p_windowHeight)
{
	float absoluteX = screenBounds.xy.x / p_windowWidth;
	float absoluteY = screenBounds.xy.y / p_windowHeight;
	float absoluteW = screenBounds.wh.x / p_windowWidth;
	float absoluteH = screenBounds.wh.y / p_windowHeight;
	Rect newRect = Rect(absoluteX, absoluteY, absoluteW, absoluteH);
	absoluteBounds = newRect;
}

Widget* Widget::findChild(std::string_view p_childID)
{
	for (auto child : m_children) {
		if (child->m_ID == p_childID) {
			return child;
		}
	}
	return nullptr;
}

Widget* Widget::findChildRecursive(std::string_view p_childID)
{
	for (auto child : m_children) {
		if (child->m_ID == p_childID) {
			return child;
		}
	}
	// if surface level search was unsuccessful
	// kinda an n^2 search though, but the tree is sparse so it's ok
	for (auto child : m_children) {
		Widget* result = child->findChild(p_childID);
		if (result) {
			return result;
		}
	}
	return nullptr;
}

bool Widget::removeChild(std::string_view p_childID)
{
	Widget* childPtr = findChild(p_childID);
	if (!childPtr) return false;
	auto itr = std::find(m_children.begin(), m_children.end(), childPtr);
	m_children.erase(itr);
	return true;
}

bool Widget::removeChildRecursive(std::string_view p_childID)
{
	if (removeChild(p_childID)) return true;
	for (auto child : m_children) {
		if (removeChildRecursive(p_childID)) return true;
	}
	return false;
}
