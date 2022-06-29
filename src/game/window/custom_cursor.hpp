#pragma once

#include <SFML/Graphics.hpp>
#include "../hud/log.hpp"
#include "../consts.h"

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
