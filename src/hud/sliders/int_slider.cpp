// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023-2024 h67ma <szycikm@gmail.com>

#include "int_slider.hpp"

#include <cassert>
#include <cmath>

#include <string>

IntSlider::IntSlider(enum SliderOrientation orientation, uint sliderLength, const sf::Font& font, bool showValueText,
					 int minVal, int defaultVal, int maxVal) :
	Slider(orientation, sliderLength, font, showValueText),
	minVal(minVal),
	maxVal(maxVal),
	currentVal(defaultVal),
	possibleValCnt(maxVal - minVal)
{
	assert(minVal < maxVal);
	assert(defaultVal >= minVal && defaultVal <= maxVal);

	this->updateText();
	this->handleSettingsChange();
}

void IntSlider::setValueFromMouse(int mouseValue)
{
	this->currentVal = (mouseValue * this->possibleValCnt / this->adjustedPossibleMouseValCnt) + this->minVal;

	this->updateText();
}

int IntSlider::getValue() const
{
	return this->currentVal;
}

void IntSlider::setValue(int value)
{
	this->currentVal = value;
	this->updateText();
	this->updateHandle();
}

void IntSlider::updateText()
{
	if (!this->showValueText)
		return;

	this->currValueText.setString(std::to_string(this->currentVal));
}

void IntSlider::updateHandle()
{
	float pos =
		std::ceil(static_cast<float>(this->currentVal - this->minVal) / static_cast<float>(this->possibleValCnt) *
				  static_cast<float>(this->adjustedPossibleMouseValCnt));

	if (this->orientation == SLIDER_HORIZONTAL)
		this->handle.setPosition(pos, 0);
	else
		this->handle.setPosition(0, pos);
}
