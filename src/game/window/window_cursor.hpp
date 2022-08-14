#pragma once

#include <SFML/Graphics.hpp>
#include "custom_cursor.hpp"

#define CURSORS_CNT 4

enum CursorType {
	POINTER,
	CROSSHAIR_WHITE,
	CROSSHAIR_YELLOW,
	CROSSHAIR_RED
};

class WindowCursor
{
	private:
		CustomCursor cursors[CURSORS_CNT];

	public:
		WindowCursor();
		bool loadCursors(bool preferCustom);
		void setCursor(sf::RenderWindow *window, CursorType type);
};
