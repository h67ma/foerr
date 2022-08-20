#pragma once

#include <SFML/Graphics.hpp>
#include "../consts.hpp"

class Color : public sf::Color
{
	public:
		Color() : sf::Color() {};
		Color(uint color) : sf::Color(color) {};
		bool loadFromColorString(std::string input);
		std::string toString();
};
