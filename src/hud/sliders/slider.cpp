// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#include "slider.hpp"

#include <string>

#include "../../consts.hpp"
#include "../../settings/settings_manager.hpp"
#include "../../util/util.hpp"
#include "../hud.hpp"

constexpr uint SLIDER_TEXT_X = 220;
constexpr uchar SLIDER_COLOR_DIM_FACTOR = 200;

constexpr uint SLIDER_WIDTH = 215;
constexpr uint SLIDER_HANDLE_HALF = SLIDER_HANDLE_WIDTH / 2;
constexpr uint SLIDER_MOUSE_POSSIBLE_VALS = SLIDER_WIDTH - SLIDER_HANDLE_WIDTH;

const sf::Vector2f sliderOutlineSize(SLIDER_WIDTH, SLIDER_HANDLE_HEIGHT);
constexpr float SLIDER_OUTLINE_THICKNESS = 1;

uint Slider::adjustedHandleHalf;
uint Slider::adjustedPossibleMouseValCnt;

Slider::Slider(const sf::Font& font) : currValueText(font, FONT_H3)
{
	this->sliderOutline.setFillColor(sf::Color::Transparent);
}

/**
 * Calculates coefficients according to GUI scale to be used elsewhere, so that multiplications/divisions
 * don't have to happen on every call.
 */
void Slider::calculateCoeffs()
{
	Slider::adjustedHandleHalf = SLIDER_HANDLE_HALF * SettingsManager::guiScale;
	Slider::adjustedPossibleMouseValCnt = std::ceil(SLIDER_MOUSE_POSSIBLE_VALS * SettingsManager::guiScale);
}

/**
 * @return true if event was consumed
 * @return false if event was not consumed
 */
bool Slider::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getPosition();

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

	// we don't need to subtract y here
	mousePos.x -= this->getPosition().x;

	this->setSliderPos(mousePos.x);
	return true;
}

void Slider::handleSettingsChange()
{
	this->updateHandle();

	this->handleGuiScaleChange();

	this->sliderOutline.setOutlineThickness(calculateGuiAwareScalar(SLIDER_OUTLINE_THICKNESS));

	this->sliderOutline.setSize(calculateGuiAwarePoint(sliderOutlineSize));

	this->sliderOutline.setOutlineColor(DIM_COLOR(SettingsManager::hudColor, SLIDER_COLOR_DIM_FACTOR));

	this->currValueText.setPosition(calculateGuiAwarePoint({ SLIDER_TEXT_X, getFontVOffset(FONT_H3) }));

	this->currValueText.handleSettingsChange();
	this->currValueText.setFillColor(SettingsManager::hudColor);

	this->handle.handleSettingsChange();
}

void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->sliderOutline, states);
	target.draw(this->currValueText, states);
	target.draw(this->handle, states);
}
