#include "slider.hpp"

#include <string>

#include "../../settings/settings_manager.hpp"
#include "slider_handle.hpp"
#include "../hud.hpp"

#define SLIDER_WIDTH 215
#define SLIDER_TEXT_X 220

#define SLIDER_COLOR_DIM_FACTOR 200

#define SLIDER_HANDLE_HALF (SLIDER_HANDLE_WIDTH / 2)
#define SLIDER_MOUSE_MAX (SLIDER_WIDTH - SLIDER_HANDLE_HALF)
#define SLIDER_MOUSE_POSSIBLE_VALS (SLIDER_WIDTH - SLIDER_HANDLE_WIDTH)

Slider::Slider(const sf::Font &font, int minVal, int maxVal, int defaultVal) :
	minVal(minVal),
	maxVal(maxVal),
	currentVal(defaultVal),
	possibleValCnt(maxVal - minVal)
{
	assert(minVal < maxVal);
	assert(defaultVal >= minVal && defaultVal <= maxVal);

	this->sliderOutline.setSize({ SLIDER_WIDTH, SLIDER_HANDLE_HEIGHT });
	this->sliderOutline.setFillColor(sf::Color::Transparent);
	this->sliderOutline.setOutlineThickness(1.F);

	this->currValueText.setFont(font);

	this->handleSettingsChange();

	this->updateDisplay();
}

int Slider::getValue() const
{
	return this->currentVal;
}

void Slider::setValue(int value)
{
	this->currentVal = value;
	this->updateDisplay();
}

void Slider::updateDisplay()
{
	this->currValueText.setString(std::to_string(this->currentVal));
	this->handle.setPosition(static_cast<float>(this->currentVal - this->minVal) /
							 this->possibleValCnt * SLIDER_MOUSE_POSSIBLE_VALS, 0);
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

void Slider::setSliderPos(int mouseX)
{
	mouseX -= SLIDER_HANDLE_HALF;

	if (mouseX < 0)
		mouseX = 0;
	
	if (mouseX > SLIDER_MOUSE_POSSIBLE_VALS)
		mouseX = SLIDER_MOUSE_POSSIBLE_VALS;

	this->currentVal = ((mouseX * possibleValCnt) / SLIDER_MOUSE_POSSIBLE_VALS) + this->minVal;
	this->updateDisplay();
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
