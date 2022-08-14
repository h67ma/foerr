#pragma once

#include <SFML/Graphics.hpp>
#include "../consts.hpp"

class CustomCursor
{
	public:
		sf::Cursor cursor;
		const char* path;
		uint hotX;
		uint hotY;
		sf::Cursor::Type fallbackCursor;
		bool load(bool preferCustom);
};
