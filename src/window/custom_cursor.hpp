#pragma once

#include <SFML/Window/Cursor.hpp>
#include "../consts.hpp"

class CustomCursor
{
	private:
		const char* path;
		const sf::Vector2u hotPoint;
		const sf::Cursor::Type fallbackCursor;
		sf::Cursor cursor;

	public:
		CustomCursor(const char* path, sf::Vector2u hotPoint, sf::Cursor::Type fallback);
		bool load(bool preferCustom);
		sf::Cursor& getCursor();
};
