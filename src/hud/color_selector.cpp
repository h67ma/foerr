#include "color_selector.hpp"

#include "../settings/settings_manager.hpp"
#include "clickable.hpp"

const sf::Vector2f secondLabelPosition(0, 25);
const sf::Vector2f thirdLabelPosition(0, 50);
const sf::Vector2f labelSize(SLIDER_HANDLE_HEIGHT, SLIDER_HANDLE_HEIGHT);

const sf::Vector2f firstSliderPosition(20, 0);
const sf::Vector2f secondSliderPosition(20, 25);
const sf::Vector2f thirdSliderPosition(20, 50);

const sf::Vector2f colorPreviewPosition(280, 0);
const sf::Vector2f colorPreviewSize(65, 65);

ColorSelector::ColorSelector(const sf::Font &font, sf::Color initialColor) :
	sliderR(font, 0, initialColor.r, COLOR_MAX_CHANNEL_VALUE),
	sliderG(font, 0, initialColor.g, COLOR_MAX_CHANNEL_VALUE),
	sliderB(font, 0, initialColor.b, COLOR_MAX_CHANNEL_VALUE)
{
	this->labelR.setSize(labelSize);
	this->labelR.setFillColor(sf::Color::Red);

	this->labelG.setPosition(secondLabelPosition);
	this->labelG.setSize(labelSize);
	this->labelG.setFillColor(sf::Color::Green);

	this->labelB.setPosition(thirdLabelPosition);
	this->labelB.setSize(labelSize);
	this->labelB.setFillColor(sf::Color::Blue);

	this->sliderR.setPosition(firstSliderPosition);
	this->sliderG.setPosition(secondSliderPosition);
	this->sliderB.setPosition(thirdSliderPosition);

	this->selectedColorPreview.setPosition(colorPreviewPosition);
	this->selectedColorPreview.setSize(colorPreviewSize);
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
	clickPos -= this->getPosition();

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
	mousePos -= this->getPosition();

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
	this->handleGuiScaleChange();

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
