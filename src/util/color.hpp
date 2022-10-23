#pragma once

#include <string>
#include <SFML/Graphics/Color.hpp>
#include "../consts.hpp"

class Color : public sf::Color
{
	public:
		Color() : sf::Color() {}
		explicit Color(uint color) : sf::Color(color) {}
		bool loadFromColorString(std::string input);
		std::string toString();
};
