#include "Button.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include "util/ext/glm/gtc/matrix_transform.hpp"
Button::Button(float p_x, float p_y, float p_w, float p_h)
{
	localBounds = Rect(p_x, p_y, p_w, p_h);
	screenBounds = queryScreenRect(localBounds);
	m_sprite.attachShader(&GenericShaders::Get().solidColorShader);
}

void Button::setText(std::string_view p_text)
{
	innerText = p_text;
}

void Button::onClick(std::function<void()> p_callback)
{
	onClickFunc = p_callback;
}

void Button::draw(DrawSurface& p_target, DrawStates& p_states)
{
	m_sprite.setBounds(Rect(0.f, 0.f, screenBounds.wh.x, screenBounds.wh.y));
	m_sprite.setPosition(glm::vec3(screenBounds.xy.x, screenBounds.xy.y, 1.f));
	GenericShaders::Get().solidColorShader.setVec3Uniform(1, testColor);
	m_sprite.draw(p_target, p_states);
	for (Widget* w : m_children) {
		w->draw(p_target, p_states);
	}
}

void Button::onUpdate(GUIEvent e)
{
	if (e.mouse.wasClick) {
		if (e.mouse.mouseState & SDL_BUTTON_LMASK && screenBounds.contains(e.mouse.x, e.mouse.y)) {
			onClickFunc();
		}
	}
	Widget::onUpdate(e);
}
