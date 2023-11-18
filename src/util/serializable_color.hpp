// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <SFML/Graphics/Color.hpp>

class SerializableColor : public sf::Color
{
	public:
		SerializableColor() : sf::Color() {}
		SerializableColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a = 0xFF) : sf::Color(r, g, b, a) {}
		SerializableColor& operator=(const sf::Color &color);
		bool loadFromColorString(const std::string &input);
		std::string toString() const;
};
