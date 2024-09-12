// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023-2024 h67ma <szycikm@gmail.com>

#include "slider.hpp"

#include <string>

#include "../../consts.hpp"
#include "../../settings/settings_manager.hpp"
#include "../../util/util.hpp"
#include "../hud.hpp"

constexpr float SLIDER_TEXT_X = 220;
constexpr uint SLIDER_TEXT_Y_PADDING = 5;
constexpr uchar SLIDER_COLOR_DIM_FACTOR = 200;

constexpr uint SLIDER_LENGTH = 215;
constexpr uint SLIDER_HANDLE_HALF = SLIDER_HANDLE_LENGTH / 2;
constexpr uint SLIDER_MOUSE_POSSIBLE_VALS = SLIDER_LENGTH - SLIDER_HANDLE_LENGTH;

const sf::Vector2f hSliderOutlineSize(SLIDER_LENGTH, SLIDER_HANDLE_THICKNESS);
const sf::Vector2f vSliderOutlineSize(SLIDER_HANDLE_THICKNESS, SLIDER_LENGTH);
constexpr float SLIDER_OUTLINE_THICKNESS = 1;

uint Slider::adjustedHandleHalf;
uint Slider::adjustedPossibleMouseValCnt;

Slider::Slider(enum SliderOrientation orientation, const sf::Font& font, bool showValueText) :
	currValueText(font, FONT_H3),
	showValueText(showValueText),
	orientation(orientation),
	handle(orientation)
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

		if (this->orientation == SLIDER_HORIZONTAL)
			this->setSliderPosH(clickPos.x);
		else
			this->setSliderPosV(clickPos.y);

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

	if (this->orientation == SLIDER_HORIZONTAL)
	{
		// we don't need to subtract y
		mousePos.x -= this->getPosition().x;

		this->setSliderPosH(mousePos.x);
	}
	else
	{
		// we don't need to subtract x
		mousePos.y -= this->getPosition().y;

		this->setSliderPosV(mousePos.y);
	}

	return true;
}

int Slider::trimSliderPos(int mouseValue)
{
	mouseValue -= Slider::adjustedHandleHalf;

	if (mouseValue < 0)
		mouseValue = 0;

	if (mouseValue > Slider::adjustedPossibleMouseValCnt)
		mouseValue = Slider::adjustedPossibleMouseValCnt;

	return mouseValue;
}

void Slider::setSliderPosH(int mouseX)
{
	mouseX = trimSliderPos(mouseX);
	this->setValueFromMouse(mouseX);

	// no need to call updateHandle() since it would needlessly calculate X from currentVal, and we already have X
	this->handle.setPosition(static_cast<float>(mouseX), 0);
}

void Slider::setSliderPosV(int mouseY)
{
	mouseY = trimSliderPos(mouseY);
	this->setValueFromMouse(mouseY);

	// no need to call updateHandle() since it would needlessly calculate Y from currentVal, and we already have Y
	this->handle.setPosition(0, static_cast<float>(mouseY));
}

void Slider::handleSettingsChange()
{
	this->updateHandle();

	this->handleGuiScaleChange();

	this->sliderOutline.setOutlineThickness(calculateGuiAwareScalar(SLIDER_OUTLINE_THICKNESS));

	if (this->orientation == SLIDER_HORIZONTAL)
		this->sliderOutline.setSize(calculateGuiAwarePoint(hSliderOutlineSize));
	else
		this->sliderOutline.setSize(calculateGuiAwarePoint(vSliderOutlineSize));

	this->sliderOutline.setOutlineColor(DIM_COLOR(SettingsManager::hudColor, SLIDER_COLOR_DIM_FACTOR));

	if (this->showValueText)
	{
		if (this->orientation == SLIDER_HORIZONTAL)
			this->currValueText.setPosition(calculateGuiAwarePoint({ SLIDER_TEXT_X, getFontVOffset(FONT_H3) }));
		else
			this->currValueText.setPosition(
				calculateGuiAwarePoint({ 0, -static_cast<float>(SLIDER_TEXT_Y_PADDING + FONT_H3) }));

		this->currValueText.handleSettingsChange();
		this->currValueText.setFillColor(SettingsManager::hudColor);
	}

	this->handle.handleSettingsChange();
}

void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->sliderOutline, states);

	if (this->showValueText)
		target.draw(this->currValueText, states);

	target.draw(this->handle, states);
}
