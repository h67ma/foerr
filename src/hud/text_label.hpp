// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include "../consts.hpp"
#include "configurable_gui_component.hpp"

/**
 * sf::Text extended with the ability to adjust to GUI scale and with a few convenient constructors.
 */
class TextLabel : public sf::Text, ConfigurableGuiComponent
{
	private:
		const uint fontSize; // e.g. FONT_H1

	public:
		TextLabel(const std::string& text, const sf::Font& font, uint fontSize, const sf::Color& color);
		TextLabel(const std::string& text, const sf::Font& font, uint fontSize);
		TextLabel(const sf::Font& font, uint fontSize);
		void handleSettingsChange() override;
};
