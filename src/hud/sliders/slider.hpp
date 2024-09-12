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

/**
 * A base class for a UI component that allows inputting a value in a designated range by dragging or clicking mouse
 * on a horizontal bar. Derived classes implement either integer or floating point value sliders.
 */
class Slider : public GuiTransformable, public sf::Drawable
{
	private:
		sf::RectangleShape sliderOutline;
		bool dragging = false;
		virtual void updateHandle() = 0;
		virtual void setSliderPos(int mouseX) = 0;

	protected:
		TextLabel currValueText;
		const bool showValueText;
		SliderHandle handle;

		// coefficients can be shared between all instances, as they only depend on GUI scale.
		// they are also the same for int/float variants
		static uint adjustedHandleHalf;
		static uint adjustedPossibleMouseValCnt;

	public:
		Slider(const sf::Font& font, bool showValueText);
		static void calculateCoeffs();
		bool handleLeftClick(sf::Vector2i clickPos);
		void handleLeftClickUp();
		bool handleMouseMove(sf::Vector2i mousePos);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
