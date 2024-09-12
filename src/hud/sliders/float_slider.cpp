// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023-2024 h67ma <szycikm@gmail.com>

#include "float_slider.hpp"

#include <cassert>
#include <cmath>

#include <iomanip>
#include <sstream>

constexpr uint BASE10 = 10;

/**
 * @param decimalPlaces controls how many decimal places are displayed in text next to the slider, as well as how
 *						precisely the value can be set
 */
FloatSlider::FloatSlider(enum SliderOrientation orientation, const sf::Font& font, bool showValueText, float minVal,
						 float defaultVal, float maxVal, uint decimalPlaces) :
	Slider(orientation, font, showValueText),
	minVal(minVal),
	maxVal(maxVal),
	currentVal(defaultVal),
	decimalPlaces(decimalPlaces),
	precisionFactor(std::pow(BASE10, decimalPlaces)),
	possibleValCnt((maxVal - minVal) * std::pow(BASE10, decimalPlaces))
{
	assert(minVal < maxVal);
	assert(defaultVal >= minVal && defaultVal <= maxVal);

	this->updateText();
	this->handleSettingsChange();
}

void FloatSlider::setValueFromMouse(int mouseValue)
{
	this->currentVal = (static_cast<float>(mouseValue) * this->possibleValCnt / Slider::adjustedPossibleMouseValCnt /
						this->precisionFactor) +
					   this->minVal;

	this->currentVal = floorf(this->currentVal * this->precisionFactor) / this->precisionFactor;

	this->updateText();
}

float FloatSlider::getValue() const
{
	return this->currentVal;
}

void FloatSlider::setValue(float value)
{
	this->currentVal = value;
	this->updateText();
	this->updateHandle();
}

void FloatSlider::updateText()
{
	if (!this->showValueText)
		return;

	std::stringstream stream;
	stream << std::fixed << std::setprecision(this->decimalPlaces) << this->currentVal;
	this->currValueText.setString(stream.str());
}

void FloatSlider::updateHandle()
{
	float pos =
		std::ceil((this->currentVal - this->minVal) / static_cast<float>(this->possibleValCnt) *
				  static_cast<float>(Slider::adjustedPossibleMouseValCnt) * static_cast<float>(this->precisionFactor));

	if (this->orientation == SLIDER_HORIZONTAL)
		this->handle.setPosition(pos, 0);
	else
		this->handle.setPosition(0, pos);
}
