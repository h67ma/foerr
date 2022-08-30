#pragma once

#include <SFML/Graphics.hpp>
#include "custom_cursor.hpp"

enum CursorType {
	POINTER,
	CROSSHAIR_WHITE,
	CROSSHAIR_YELLOW,
	CROSSHAIR_RED,
	_CURSORS_CNT
};

class WindowCursor
{
	private:
		CustomCursor cursors[_CURSORS_CNT] = {
			CustomCursor(PATH_CURSOR_ARROW, 1, 1, sf::Cursor::Arrow),
			CustomCursor(PATH_CURSOR_CROSS_WHITE, 13, 13, sf::Cursor::Cross),
			CustomCursor(PATH_CURSOR_CROSS_YELLOW, 13, 13, sf::Cursor::Cross),
			CustomCursor(PATH_CURSOR_CROSS_RED, 13, 13, sf::Cursor::Cross)
		};

	public:
		bool loadCursors(bool preferCustom);
		void setCursor(sf::RenderWindow &window, CursorType type);
};
