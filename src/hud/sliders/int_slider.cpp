// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#include "int_slider.hpp"

#include <cassert>
#include <string>

IntSlider::IntSlider(const sf::Font& font, int minVal, int defaultVal, int maxVal) :
	Slider(font),
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

void IntSlider::setSliderPos(int mouseX)
{
	mouseX -= Slider::adjustedHandleHalf;

	if (mouseX < 0)
		mouseX = 0;

	if (mouseX > Slider::adjustedPossibleMouseValCnt)
		mouseX = Slider::adjustedPossibleMouseValCnt;

	this->currentVal = (mouseX * this->possibleValCnt / Slider::adjustedPossibleMouseValCnt) + this->minVal;

	this->updateText();

	// no need to call updateHandle() since it would needlessly calculate X from currentVal, and we already have X
	this->handle.setPosition(static_cast<float>(mouseX), 0);
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
	this->currValueText.setString(std::to_string(this->currentVal));
}

void IntSlider::updateHandle()
{
	float x = static_cast<float>(this->currentVal - this->minVal) /
								 this->possibleValCnt * Slider::adjustedPossibleMouseValCnt;
	this->handle.setPosition(x, 0);
}
