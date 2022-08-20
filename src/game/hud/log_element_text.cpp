#include <SFML/Graphics.hpp>
#include "log_element_text.hpp"

LogElementText::LogElementText(std::string text, sf::Font &font, GuiScale scale, sf::Color color)
{
	this->setString(text);
	this->setFont(font);
	this->setFillColor(color);
	this->setGuiScale(scale);
}

void LogElementText::setGuiScale(GuiScale scale)
{
	switch (scale)
	{
		case GUI_SMALL:
			this->setCharacterSize(FONT_SIZE_SMALL);
			break;
		case GUI_LARGE:
			this->setCharacterSize(FONT_SIZE_LARGE);
			break;
		case GUI_NORMAL:
		default:
			this->setCharacterSize(FONT_SIZE_NORMAL);
	}
}

bool LogElementText::isTimeUp()
{
	return this->clock.getElapsedTime().asSeconds() > LOG_ELEMENT_LIFE_TIME_S;
}
