#include "log_element_text.hpp"

#include <string>

#include "hud.hpp"

#define LOG_ELEMENT_LIFE_TIME_S 5

LogElementText::LogElementText(const std::string &text, sf::Font &font, float scale, sf::Color color)
{
	this->setString(text);
	this->setFont(font);
	this->setFillColor(color);
	this->setGuiScale(scale);
}

void LogElementText::setGuiScale(float scale)
{
	this->setCharacterSize(static_cast<uint>(scale * FONT_H3));
}

bool LogElementText::isTimeUp()
{
	return this->clock.getElapsedTime().asSeconds() > LOG_ELEMENT_LIFE_TIME_S;
}
