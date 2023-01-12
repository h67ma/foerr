#pragma once

#include <string>
#include <SFML/Graphics/Color.hpp>
#include "../consts.hpp"

class SerializableColor : public sf::Color
{
	public:
		SerializableColor() : sf::Color() {}
		explicit SerializableColor(uint color) : sf::Color(color) {}
		bool loadFromColorString(const std::string &input);
		std::string toString();
};
