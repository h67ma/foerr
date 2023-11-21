// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Clock.hpp>

#include "text_label.hpp"

class LogElementText : public TextLabel
{
	private:
		sf::Clock clock;

	public:
		LogElementText(const std::string& text, sf::Font& font, const sf::Color& color);
		bool isTimeUp();
};
