// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "log_element_text.hpp"

#include <string>

#include "hud.hpp"
#include "text_label.hpp"

constexpr float LOG_ELEMENT_LIFE_TIME_S = 5;

LogElementText::LogElementText(const std::string& text, const sf::Font& font, const sf::Color& color) :
	TextLabel(text, font, FONT_H3, color)
{
}

bool LogElementText::isTimeUp() const
{
	return this->clock.getElapsedTime().asSeconds() > LOG_ELEMENT_LIFE_TIME_S;
}
