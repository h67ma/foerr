#pragma once

#include <SFML/Graphics.hpp>
#include "../hud/log.hpp"
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
		sf::Window *window;
		CustomCursor cursors[CURSORS_CNT];

	public:
		WindowCursor(sf::Window *window, Log *log);
		bool loadCursors(bool preferCustom);
		void setCursor(CursorType type);
};
