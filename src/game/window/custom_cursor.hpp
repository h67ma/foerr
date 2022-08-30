#pragma once

#include <SFML/Graphics.hpp>
#include "../consts.hpp"

class CustomCursor
{
	private:
		const char* path;
		uint hotX;
		uint hotY;
		sf::Cursor::Type fallbackCursor;
		sf::Cursor cursor;

	public:
		CustomCursor(const char* path, uint hotX, uint hotY, sf::Cursor::Type fallback);
		bool load(bool preferCustom);
		sf::Cursor& getCursor();
};
