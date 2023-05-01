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
		void setSliderPos(int mouseX) override;

	public:
		FloatSlider(const sf::Font &font, float minVal, float defaultVal, float maxVal, uint decimalPlaces);
		float getValue() const;
		void setValue(float value);
};
