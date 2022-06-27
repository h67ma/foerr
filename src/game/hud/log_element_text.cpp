#include <SFML/Graphics.hpp>
#include "log_element_text.hpp"

LogElementText::LogElementText(std::string text, sf::Font *font, uint fontSize, sf::Color color)
{
	this->setString(text);
	this->setFont(*font);
	this->setCharacterSize(fontSize);
	this->setFillColor(color);
}

bool LogElementText::isTimeUp()
{
	return this->clock.getElapsedTime().asSeconds() > LOG_ELEMENT_LIFE_TIME_S;
}
