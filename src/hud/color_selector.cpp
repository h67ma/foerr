// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023-2024 h67ma <szycikm@gmail.com>

#include "color_selector.hpp"

#include "../settings/settings_manager.hpp"
#include "../util/util.hpp"
#include "clickable.hpp"

const sf::Vector2f POS_2ND_LABEL(0, 25);
const sf::Vector2f POS_3RD_LABEL(0, 50);
const sf::Vector2f SIZE_LABEL(SLIDER_HANDLE_THICKNESS, SLIDER_HANDLE_THICKNESS);

const sf::Vector2f POS_1ST_SLIDER(20, 0);
const sf::Vector2f POS_2ND_SLIDER(20, 25);
const sf::Vector2f POS_3RD_SLIDER(20, 50);

const sf::Vector2f POS_COLOR_PREVIEW(280, 0);
const sf::Vector2f SIZE_COLOR_PREVIEW(65, 65);
constexpr float COLOR_PREVIEW_OUTLINE_THICKNESS = 1;

ColorSelector::ColorSelector(const sf::Font& font, sf::Color initialColor) :
	sliderR(SLIDER_HORIZONTAL, font, true, 0, initialColor.r, COLOR_MAX_CHANNEL_VALUE),
	sliderG(SLIDER_HORIZONTAL, font, true, 0, initialColor.g, COLOR_MAX_CHANNEL_VALUE),
	sliderB(SLIDER_HORIZONTAL, font, true, 0, initialColor.b, COLOR_MAX_CHANNEL_VALUE)
{
	this->labelR.setFillColor(sf::Color::Red);
	this->labelG.setFillColor(sf::Color::Green);
	this->labelB.setFillColor(sf::Color::Blue);

	this->sliderR.setPosition(POS_1ST_SLIDER);
	this->sliderG.setPosition(POS_2ND_SLIDER);
	this->sliderB.setPosition(POS_3RD_SLIDER);

	this->selectedColorPreview.setOutlineThickness(COLOR_PREVIEW_OUTLINE_THICKNESS);

	this->handleSettingsChange();

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

	// labelR is in (0, 0), no need to adjust position
	this->labelR.setSize(SIZE_LABEL * SettingsManager::guiScale);

	this->labelG.setPosition(POS_2ND_LABEL * SettingsManager::guiScale);
	this->labelG.setSize(SIZE_LABEL * SettingsManager::guiScale);

	this->labelB.setPosition(POS_3RD_LABEL * SettingsManager::guiScale);
	this->labelB.setSize(SIZE_LABEL * SettingsManager::guiScale);

	this->selectedColorPreview.setPosition(POS_COLOR_PREVIEW * SettingsManager::guiScale);
	this->selectedColorPreview.setSize(SIZE_COLOR_PREVIEW * SettingsManager::guiScale);
	this->selectedColorPreview.setOutlineColor(SettingsManager::hudColor);
}

void ColorSelector::draw(sf::RenderTarget& target, sf::RenderStates states) const
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
