#pragma once

#include <SFML/System/Vector2.hpp>

class Hoverable
{
	public:
		virtual bool containsPoint(sf::Vector2i coords) = 0;
		virtual void setHover(bool hover) = 0;
};
