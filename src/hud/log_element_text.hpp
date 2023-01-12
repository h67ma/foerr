#pragma once

#include <string>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include "hud.hpp"
#include "../consts.hpp"

class LogElementText: public sf::Text
{
	private:
		sf::Clock clock;

	public:
		LogElementText(const std::string &text, sf::Font &font, GuiScale scale, sf::Color color);
		void setGuiScale(GuiScale scale);
		bool isTimeUp();
};
