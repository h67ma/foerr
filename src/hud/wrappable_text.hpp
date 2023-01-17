#pragma once

#include <string>
#include <SFML/Graphics/Text.hpp>

class WrappableText : public sf::Text
{
	public:
		void setString(std::string text, uint maxWidth);
};
