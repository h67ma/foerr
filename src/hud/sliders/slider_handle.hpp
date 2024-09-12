// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include "../../consts.hpp"
#include "../configurable_gui_component.hpp"
#include "slider_orientation.hpp"

constexpr float SLIDER_HANDLE_THICKNESS = 15;
constexpr float SLIDER_HANDLE_LENGTH = 21;

constexpr uint SLIDER_HANDLE_THINGY_VERT_CNT = 8;

/**
 * Represents the draggable handle used to input value into Slider. Could be replaced by a sprite.
 */
class SliderHandle : public sf::Drawable, public sf::Transformable, public ConfigurableGuiComponent
{
	private:
		sf::RectangleShape outlineRect;
		const enum SliderOrientation orientation;

		// what else to call something that looks like this |||| ? it's a thingy.
		sf::VertexArray thingy = sf::VertexArray(sf::Lines, SLIDER_HANDLE_THINGY_VERT_CNT);

	public:
		explicit SliderHandle(enum SliderOrientation orientation);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
