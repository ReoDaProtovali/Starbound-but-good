#pragma once
#include "Widget.hpp"
#include "util/Rect.hpp"
#include <string>
#include <string_view>
#include <functional>
#include "Framework/Graphics/Sprite.hpp"
#include "Framework/Graphics/Text.hpp"
#include "DefaultFonts.hpp"
class Button : public Widget {
public:
	Button(float p_x, float p_y, float p_w, float p_h);
	void setText(std::string_view p_text);
	void onClick(std::function<void()> p_callback);
	void onHover(std::function<void(bool)> p_callback);

	void draw(DrawSurface& p_target, DrawStates& p_states) override;
	void onUpdate(GUIEvent e) override;

	glm::vec3 testColor{ 0.2f };
	std::string innerText = "click me";
	Text buttonText{ DefaultFonts.videotype, innerText };
	bool disabled = false;
protected:
	std::function<void()> onClickFunc;
	std::function<void(bool)> onHoverFunc;
	Sprite m_sprite{ glm::vec3(screenBounds.xy.x, screenBounds.xy.y, 0.f), Rect(0.f, 0.f, screenBounds.wh.x, screenBounds.wh.y) };
};