// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "log_element_text.hpp"

#include <string>

#include "hud.hpp"
#include "text_label.hpp"

#define LOG_ELEMENT_LIFE_TIME_S 5

LogElementText::LogElementText(const std::string &text, sf::Font &font, const sf::Color &color) :
	TextLabel(text, font, FONT_H3, color)
{
	this->setString(text);
	this->setFont(font);
	this->setFillColor(color);
}

bool LogElementText::isTimeUp()
{
	return this->clock.getElapsedTime().asSeconds() > LOG_ELEMENT_LIFE_TIME_S;
}
