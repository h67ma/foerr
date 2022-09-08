#include <SFML/Graphics.hpp>
#include "log_element_text.hpp"
#include "../util/util.hpp"

LogElementText::LogElementText(std::string text, sf::Font &font, GuiScale scale, sf::Color color)
{
	this->setString(text);
	this->setFont(font);
	this->setFillColor(color);
	this->setGuiScale(scale);
}

void LogElementText::setGuiScale(GuiScale scale)
{
	this->setCharacterSize(getFontSize(scale, FONT_H3));
}

bool LogElementText::isTimeUp()
{
	return this->clock.getElapsedTime().asSeconds() > LOG_ELEMENT_LIFE_TIME_S;
}
