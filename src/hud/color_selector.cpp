#include "color_selector.hpp"

#include "../settings/settings_manager.hpp"
#include "clickable.hpp"

#define MAX_CHANNEL_VALUE 0xFF

#define SECOND_ROW_Y 25
#define THIRD_ROW_Y 50

#define CHANNEL_SLIDER_X 20

#define COLOR_PREVIEW_X 280
#define COLOR_PREVIEW_SIDE_LEN 65

ColorSelector::ColorSelector(const sf::Font &font) :
	sliderR(font, 0, MAX_CHANNEL_VALUE, SettingsManager::hudColor.r),
	sliderG(font, 0, MAX_CHANNEL_VALUE, SettingsManager::hudColor.g),
	sliderB(font, 0, MAX_CHANNEL_VALUE, SettingsManager::hudColor.b)
{
	this->labelR.setSize({ SLIDER_HANDLE_HEIGHT, SLIDER_HANDLE_HEIGHT });
	this->labelR.setFillColor(sf::Color::Red);

	this->labelG.setPosition(0.F, SECOND_ROW_Y);
	this->labelG.setSize({ SLIDER_HANDLE_HEIGHT, SLIDER_HANDLE_HEIGHT });
	this->labelG.setFillColor(sf::Color::Green);

	this->labelB.setPosition(0.F, THIRD_ROW_Y);
	this->labelB.setSize({ SLIDER_HANDLE_HEIGHT, SLIDER_HANDLE_HEIGHT });
	this->labelB.setFillColor(sf::Color::Blue);

	this->sliderR.setPosition(CHANNEL_SLIDER_X, 0.F);
	this->sliderG.setPosition(CHANNEL_SLIDER_X, SECOND_ROW_Y);
	this->sliderB.setPosition(CHANNEL_SLIDER_X, THIRD_ROW_Y);

	this->selectedColorPreview.setPosition(COLOR_PREVIEW_X, 0.F);
	this->selectedColorPreview.setSize({ COLOR_PREVIEW_SIDE_LEN, COLOR_PREVIEW_SIDE_LEN });
	this->selectedColorPreview.setOutlineColor(SettingsManager::hudColor);
	this->selectedColorPreview.setOutlineThickness(1.F);

	this->updateColor(); // initial color
}

sf::Color ColorSelector::getSelectedColor() const
{
	return this->selectedColor;
}

void ColorSelector::setSelectedColor(sf::Color color)
{
	this->selectedColor = color;

	this->sliderR.setValue(color.r);
	this->sliderG.setValue(color.g);
	this->sliderB.setValue(color.b);

	this->selectedColorPreview.setFillColor(color);
}

/**
 * Updates the internal color value and color preview rectangle based on the three slider values.
 */
void ColorSelector::updateColor()
{
	this->selectedColor.r = this->sliderR.getValue();
	this->selectedColor.g = this->sliderG.getValue();
	this->selectedColor.b = this->sliderB.getValue();

	this->selectedColorPreview.setFillColor(this->selectedColor);
}

bool ColorSelector::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getIntPosition();

	if (this->sliderR.handleLeftClick(clickPos))
		return true;

	if (this->sliderG.handleLeftClick(clickPos))
		return true;

	if (this->sliderB.handleLeftClick(clickPos))
		return true;

	return false;
}

void ColorSelector::handleLeftClickUp()
{
	this->sliderR.handleLeftClickUp();
	this->sliderG.handleLeftClickUp();
	this->sliderB.handleLeftClickUp();
}

bool ColorSelector::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getIntPosition();

	if (this->sliderR.handleMouseMove(mousePos))
	{
		this->updateColor();
		return true;
	}

	if (this->sliderG.handleMouseMove(mousePos))
	{
		this->updateColor();
		return true;
	}

	if (this->sliderB.handleMouseMove(mousePos))
	{
		this->updateColor();
		return true;
	}

	return false;
}

void ColorSelector::handleSettingsChange()
{
	this->sliderR.handleSettingsChange();
	this->sliderG.handleSettingsChange();
	this->sliderB.handleSettingsChange();

	this->selectedColorPreview.setOutlineColor(SettingsManager::hudColor);
}

void ColorSelector::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->labelR, states);
	target.draw(this->labelG, states);
	target.draw(this->labelB, states);

	target.draw(this->sliderR, states);
	target.draw(this->sliderG, states);
	target.draw(this->sliderB, states);

	target.draw(this->selectedColorPreview, states);
}
