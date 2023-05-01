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
		void setSliderPos(int mouseX) override;

	public:
		IntSlider(const sf::Font &font, int minVal, int defaultVal, int maxVal);
		int getValue() const;
		void setValue(int value);
};
