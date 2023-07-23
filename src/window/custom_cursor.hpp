#pragma once

#include <string>

#include <SFML/Window/Cursor.hpp>
#include "../consts.hpp"

class CustomCursor : public sf::Cursor
{
	private:
		const std::string path;
		const sf::Vector2u hotPoint;
		const sf::Cursor::Type fallbackCursor;

	public:
		CustomCursor(const std::string &path, sf::Vector2u hotPoint, sf::Cursor::Type fallback);
		bool load();
};
