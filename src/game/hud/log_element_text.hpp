#pragma once

#include <SFML/Graphics.hpp>
#include "hud.hpp"
#include "../consts.hpp"

#define LOG_ELEMENT_LIFE_TIME_S 5

class LogElementText: public sf::Text
{
	private:
		sf::Clock clock;

	public:
		LogElementText(std::string text, sf::Font &font, GuiScale scale, sf::Color color);
		void setScale(GuiScale scale);
		bool isTimeUp();
};
