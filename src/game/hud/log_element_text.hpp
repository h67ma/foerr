#pragma once

#include <SFML/Graphics.hpp>

#define LOG_ELEMENT_LIFE_TIME_S 3

class LogElementText: public sf::Text
{
	private:
		sf::Clock clock;

	public:
		LogElementText(std::string text, sf::Font *font, uint fontSize, sf::Color color);
		bool isTimeUp();
};
