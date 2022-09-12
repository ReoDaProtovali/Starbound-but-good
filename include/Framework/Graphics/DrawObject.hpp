
#ifndef DRAWOBJECT_H
#define DRAWOBJECT_H
#include "Framework/Graphics/DrawStates.hpp"
#include "Framework/Graphics/DrawSurface.hpp"

//class DrawSurface;
// Polymorphic Interface to allow draw surfaces to draw classes that inherit from this, regardless of higher type
struct DrawObject {
	DrawObject() {};
	
	virtual void draw(DrawSurface& p_target, DrawStates& p_drawStates) const = 0;
};

#endif