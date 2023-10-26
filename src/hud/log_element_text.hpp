#pragma once

#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Clock.hpp>

#include "text_label.hpp"

class LogElementText : public TextLabel
{
	private:
		sf::Clock clock;

	public:
		LogElementText(const std::string &text, sf::Font &font, const sf::Color &color);
		bool isTimeUp();
};
