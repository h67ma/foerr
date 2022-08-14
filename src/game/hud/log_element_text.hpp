#pragma once

#include <SFML/Graphics.hpp>
#include "../consts.hpp"

#define LOG_ELEMENT_LIFE_TIME_S 5

class LogElementText: public sf::Text
{
	private:
		sf::Clock clock;

	public:
		LogElementText(std::string text, sf::Font *font, uint fontSize, sf::Color color);
		bool isTimeUp();
};
