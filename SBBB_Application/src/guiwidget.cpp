#include "GUIWidget.hpp"

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
	p_child->screenBounds = p_child->queryScreenRect(p_child->localBounds);
}

Rect Widget::queryScreenRect(Rect p_childRect)
{
	if (m_parent == nullptr) return p_childRect;
	// untested coordinate escalation
	float newX = m_parent->localBounds.xy.x + m_parent->localBounds.wh.x * p_childRect.xy.x;
	float newY = m_parent->localBounds.xy.y + m_parent->localBounds.wh.y * p_childRect.xy.y;
	float newW = m_parent->localBounds.wh.x * p_childRect.wh.x;
	float newH = m_parent->localBounds.wh.y * p_childRect.wh.y;
	return m_parent->queryScreenRect(Rect(newX, newY, newW, newH));
}
