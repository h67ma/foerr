#pragma once

#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>

class LogElementText: public sf::Text
{
	private:
		sf::Clock clock;

	public:
		LogElementText(const std::string &text, sf::Font &font, float scale, sf::Color color);
		void setGuiScale(float scale);
		bool isTimeUp();
};
