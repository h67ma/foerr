// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023-2024 h67ma <szycikm@gmail.com>

#pragma once

#include "slider.hpp"

/**
 * A Slider which allows inputting a floating point value.
 */
class FloatSlider : public Slider
{
	private:
		const float minVal;
		const float maxVal;
		const uint possibleValCnt;
		const uint decimalPlaces;
		const uint precisionFactor; // idk how to call this
		float currentVal;

		void updateText();
		void updateHandle() override;
		void setValueFromMouse(int mouseValue) override;

	public:
		FloatSlider(enum SliderOrientation orientation, uint sliderLength, const sf::Font& font, bool showValueText,
					float minVal, float defaultVal, float maxVal, uint decimalPlaces);
		float getValue() const;
		void setValue(float value);
};
