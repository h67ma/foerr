#pragma once

#include <SFML/Graphics.hpp>
#include "log.hpp"

class CustomCursor
{
	public:
		sf::Cursor cursor;
		Log *log;
		const char* path;
		uint hotX;
		uint hotY;
		sf::Cursor::Type fallbackCursor;
		bool load(bool preferCustom);
};
