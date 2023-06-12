#include "GUITextField.hpp"
#include "util/utils.hpp"
void GUITextField::draw(DrawSurface& p_target, DrawStates& p_states)
{
	if (m_usingScreenBounds) {
		updateScreenBounds(p_target.getViewportWidth(), p_target.getViewportHeight());
	}

	if (m_backgroundEnabled) {
		m_backgroundSprite.setBounds(Rect(0.f, 0.f, absoluteBounds.wh.x, absoluteBounds.wh.y));
		m_backgroundSprite.setPosition(glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y, 1.f));
		gs.solidColorShader.setVec3Uniform(1, backgroundColor);
		gs.solidColorShader.setFloatUniform(2, backgroundOpacity);
		m_backgroundSprite.draw(p_target, p_states);
		gs.solidColorShader.setFloatUniform(2, 1.f);
	}

	if (!m_useRelativeScaling) {
		float screenHeightProportion = m_textHeight / p_target.getViewportHeight();
		m_fieldText.setScale(glm::vec2(screenHeightProportion * (1.f / p_target.getAspect()), -screenHeightProportion));
	}
	else {
		m_fieldText.setScale(glm::vec2(absoluteBounds.wh.x * m_textHeight * (1.f / p_target.getAspect()) * (1.f / absoluteBounds.getAspect()), -absoluteBounds.wh.y * m_textHeight));
	}
	if (centered) {
		float textWidthAbsolute;
		float textHeightAbsolute;
		if (m_useRelativeScaling) { // in this case, textHeight is in local units
			textWidthAbsolute = m_fieldText.getMaxPixelWidth(m_textHeight * (absoluteBounds.wh.y * p_target.getViewportHeight())) / p_target.getViewportWidth();
			textHeightAbsolute = m_textHeight * absoluteBounds.wh.y * 0.8f; // the constant is a bit silly but looks nicer due to the font being bottom heavy
		}
		else { // in this case, text height is in pixels
			textWidthAbsolute = m_fieldText.getMaxPixelWidth(m_textHeight) / p_target.getViewportWidth();
			textHeightAbsolute = (m_textHeight / p_target.getViewportHeight()) * 0.8f;
		}
		float newX = (absoluteBounds.wh.x - textWidthAbsolute) / 2.f + absoluteBounds.xy.x;
		float newY = -(absoluteBounds.wh.y - textHeightAbsolute) / 2.f + absoluteBounds.xy.y + absoluteBounds.wh.y;

		m_fieldText.setPosition(glm::vec3(newX, newY, 0.f));
	}
	else {
		m_fieldText.setPosition(glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y + absoluteBounds.wh.y, 0.f));
	}
	m_fieldText.draw(glm::vec3(1.f), p_target, p_states);
	float test = m_fieldText.getMaxPixelWidth(m_textHeight);

	for (Widget* w : m_children) {
		w->draw(p_target, p_states);
	}
}

void GUITextField::setText(std::string_view p_text)
{
	m_textString = p_text;
	m_fieldText.setText(p_text);
}

void GUITextField::setTextHeight(float p_height)
{
	m_textHeight = p_height;
}

void GUITextField::disableBackground()
{
	m_backgroundEnabled = false;
}

void GUITextField::enableBackground()
{
	m_backgroundEnabled = true;
}

void GUITextField::disableRelativeScaling()
{
	m_useRelativeScaling = false;
}

void GUITextField::enableRelativeScaling()
{
	m_useRelativeScaling = true;
}
