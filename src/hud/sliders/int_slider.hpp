// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023-2024 h67ma <szycikm@gmail.com>

#pragma once

#include "slider.hpp"

class IntSlider : public Slider
{
	private:
		const int minVal;
		const int maxVal;
		const uint possibleValCnt;
		int currentVal;

		void updateText();
		void updateHandle() override;
		void setValueFromMouse(int mouseValue) override;

	public:
		IntSlider(enum SliderOrientation orientation, const sf::Font& font, bool showValueText, int minVal,
				  int defaultVal, int maxVal);
		int getValue() const;
		void setValue(int value);
};
