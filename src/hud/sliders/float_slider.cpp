#include "float_slider.hpp"

#include <cassert>
#include <iomanip>
#include <sstream>
#include <cmath>

constexpr uint BASE10 = 10;

/**
 * @param decimalPlaces controls how many decimal places are displayed in text next to the slider, as well as how
 *						precisely the value can be set
 */
FloatSlider::FloatSlider(const sf::Font &font, float minVal, float defaultVal, float maxVal, uint decimalPlaces) :
	Slider(font),
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
	this->updateHandle();
}

void FloatSlider::setSliderPos(int mouseX)
{
	mouseX -= SLIDER_HANDLE_HALF;

	if (mouseX < 0)
		mouseX = 0;
	
	if (mouseX > SLIDER_MOUSE_POSSIBLE_VALS)
		mouseX = SLIDER_MOUSE_POSSIBLE_VALS;

	this->currentVal = (static_cast<float>(mouseX) * this->possibleValCnt /
						SLIDER_MOUSE_POSSIBLE_VALS / this->precisionFactor) +
					   this->minVal;

	this->updateText();

	// no need to call updateHandle() since it would needlessly calculate X from currentVal, and we already have X
	this->handle.setPosition(static_cast<float>(mouseX), 0);
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
	std::stringstream stream;
	stream << std::fixed << std::setprecision(this->decimalPlaces) << this->currentVal;
	this->currValueText.setString(stream.str());
}

void FloatSlider::updateHandle()
{
	float x = (this->currentVal - this->minVal) /
			  this->possibleValCnt * SLIDER_MOUSE_POSSIBLE_VALS * this->precisionFactor;
	this->handle.setPosition(x, 0);
}
