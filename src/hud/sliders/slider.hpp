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

/**
 * A base class for a UI component that allows inputting a value in a designated range by dragging or clicking mouse
 * on a bar. Derived classes implement either integer or floating point value sliders.
 */
class Slider : public GuiTransformable, public sf::Drawable
{
	private:
		sf::RectangleShape sliderOutline;
		bool dragging = false;
		static uint adjustedHandleHalf;
		virtual void updateHandle() = 0;
		virtual void setValueFromMouse(int mouseValue) = 0;
		int trimSliderPos(int mouseValue);
		void setSliderPosV(int mouseY);
		void setSliderPosH(int mouseX);

	protected:
		const enum SliderOrientation orientation;
		TextLabel currValueText;
		const bool showValueText;
		SliderHandle handle;

		// coefficients can be shared between all instances, as they only depend on GUI scale.
		// they are also the same for int/float variants
		static uint adjustedPossibleMouseValCnt;

	public:
		Slider(enum SliderOrientation orientation, const sf::Font& font, bool showValueText);
		static void calculateCoeffs();
		bool handleLeftClick(sf::Vector2i clickPos);
		void handleLeftClickUp();
		bool handleMouseMove(sf::Vector2i mousePos);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
