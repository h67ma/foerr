// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "../gui_transformable.hpp"
#include "../text_label.hpp"
#include "slider_handle.hpp"
#include "slider_orientation.hpp"

// common length of a horizontal slider used to set some value
constexpr uint INPUT_SLIDER_LENGTH = 215;

/**
 * A base class for a UI component that allows inputting a value in a designated range by dragging or clicking mouse
 * on a bar. Derived classes implement either integer or floating point value sliders.
 */
class Slider : public GuiTransformable, public sf::Drawable
{
	private:
		sf::RectangleShape sliderOutline;
		bool dragging = false;
		uint adjustedHandleHalf;
		const uint sliderLength;
		const uint possibleMouseVals;
		virtual void updateHandle() = 0;
		virtual void setValueFromMouse(int mouseValue) = 0;
		int trimSliderPos(int mouseValue) const;
		void setSliderPosV(int mouseY);
		void setSliderPosH(int mouseX);
		void calculateCoeffs();

	protected:
		const enum SliderOrientation orientation;
		TextLabel currValueText;
		const bool showValueText;
		SliderHandle handle;
		uint adjustedPossibleMouseValCnt;

	public:
		Slider(enum SliderOrientation orientation, uint sliderLength, const sf::Font& font, bool showValueText);
		bool handleLeftClick(sf::Vector2i clickPos);
		void handleLeftClickUp();
		bool handleMouseMove(sf::Vector2i mousePos);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
