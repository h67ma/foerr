#include "slider.hpp"

#include <string>

#include "../../settings/settings_manager.hpp"
#include "../../consts.hpp"
#include "../hud.hpp"

constexpr uint SLIDER_TEXT_X = 220;
constexpr uchar SLIDER_COLOR_DIM_FACTOR = 200;

Slider::Slider(const sf::Font &font)
{
	this->sliderOutline.setSize({ SLIDER_WIDTH, SLIDER_HANDLE_HEIGHT });
	this->sliderOutline.setFillColor(sf::Color::Transparent);
	this->sliderOutline.setOutlineThickness(1.F);

	this->currValueText.setFont(font);

	this->handleSettingsChange();
}

/**
 * @return true if event was consumed
 * @return false if event was not consumed
 */
bool Slider::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getIntPosition();

	if (this->sliderOutline.getLocalBounds().contains(static_cast<sf::Vector2f>(clickPos)))
	{
		this->dragging = true;
		this->setSliderPos(clickPos.x);
		return true;
	}

	return false;
}

void Slider::handleLeftClickUp()
{
	this->dragging = false;
}

/**
 * @return true if event was consumed
 * @return false if event was not consumed
 */
bool Slider::handleMouseMove(sf::Vector2i mousePos)
{
	if (!dragging)
		return false;

	mousePos.x -= this->getIntPosition().x;

	this->setSliderPos(mousePos.x);
	return true;
}

void Slider::handleSettingsChange()
{
	this->sliderOutline.setOutlineColor(DIM_COLOR(SettingsManager::hudColor, SLIDER_COLOR_DIM_FACTOR));

	this->currValueText.setPosition(SLIDER_TEXT_X,
									getFontVOffset(SettingsManager::guiScale, FONT_H3));

	this->currValueText.setCharacterSize(static_cast<uint>(SettingsManager::guiScale * FONT_H3));
	this->currValueText.setFillColor(SettingsManager::hudColor);

	this->handle.handleSettingsChange();
}

void Slider::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->sliderOutline, states);
	target.draw(this->currValueText, states);
	target.draw(this->handle, states);
}
