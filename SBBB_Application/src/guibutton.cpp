#include "GUIButton.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include "util/ext/glm/gtc/matrix_transform.hpp"
Button::Button(std::string_view p_ID, float p_x, float p_y, float p_w, float p_h)
{
	localBounds = Rect(p_x, p_y, p_w, p_h);
	absoluteBounds = queryAbsoluteRect(localBounds);
	m_sprite.attachShader(&GenericShaders::Get().solidColorShader);
	onClickFunc = [](void) {};
	onHoverFunc = [](bool) {};
}

void Button::setText(std::string_view p_text)
{
	innerText = p_text;
	buttonText.setText(p_text);
}

void Button::onClick(std::function<void()> p_callback)
{
	onClickFunc = p_callback;
}

void Button::onHover(std::function<void(bool)> p_callback)
{
	onHoverFunc = p_callback;
}

void Button::draw(DrawSurface& p_target, DrawStates& p_states)
{
	m_sprite.setBounds(Rect(0.f, 0.f, absoluteBounds.wh.x, absoluteBounds.wh.y));
	m_sprite.setPosition(glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y, 1.f));
	GenericShaders::Get().solidColorShader.setVec3Uniform(1, testColor);
	//GenericShaders::Get().fancyShader.setFloatUniform(1, SDL_GetTicks() / 1000.f);

	m_sprite.draw(p_target, p_states);
	buttonText.setScale(glm::vec2(absoluteBounds.wh.x * 0.2, -absoluteBounds.wh.y * 0.24));
	buttonText.setPosition(glm::vec3(absoluteBounds.xy.x + 0.05 * absoluteBounds.wh.x, absoluteBounds.xy.y + 0.5 * absoluteBounds.wh.y, 0.f));
	buttonText.draw(glm::vec3(1.f), p_target, p_states);
	for (Widget* w : m_children) {
		w->draw(p_target, p_states);
	}
}

void Button::onUpdate(GUIEvent e)
{
	if (!disabled)
		if (absoluteBounds.contains(e.mouse.x, e.mouse.y)) {
			onHoverFunc(true);
		}
		else {
			onHoverFunc(false);
		}
	if (e.mouse.wasClick) {
		if ((e.mouse.mouseState & SDL_BUTTON_LMASK) && absoluteBounds.contains(e.mouse.x, e.mouse.y)) {
			onClickFunc();
		}
	}
	Widget::onUpdate(e);
}
