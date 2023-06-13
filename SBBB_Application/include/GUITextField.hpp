#pragma once
#include "GUIWidget.hpp"
#include "glm/vec3.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include <string>
#include <string_view>
#include "Framework/Graphics/Text.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "DefaultFonts.hpp"

// todo:: implement word wrap
class GUITextField : public Widget {
public:
	GUITextField(std::string_view p_ID, Rect p_localBounds) : Widget(p_ID, p_localBounds) {
		m_backgroundSprite.attachShader(&GenericShaders::Get().solidColorShader);
	};

	void draw(DrawSurface& p_target, DrawStates& p_states) override;
	void setText(std::string_view p_text);
	void appendText(std::string_view p_text);
	void setTextHeight(float p_height);
	void disableBackground();
	void enableBackground();
	void disableRelativeScaling();
	void enableRelativeScaling();

	glm::vec3 backgroundColor{ 0.2f };
	float backgroundOpacity = 1.f;
	bool centered = true;
	Text m_fieldText{ DefaultFonts.videotype, "" };
protected:
	GenericShaders& gs = GenericShaders::Get();
	std::string m_textString;
	Sprite m_backgroundSprite;
	float m_textHeight = 50; // if not using relative scaling, it is pixel height. otherwise, 0-1.
	bool m_useRelativeScaling = false; // false is pixel-based text height, true is local coord text height
	bool m_backgroundEnabled = false;
};