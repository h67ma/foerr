#pragma once

#include <SFML/Window/Window.hpp>

enum CursorType {
	POINTER,
	CROSSHAIR_WHITE,
	CROSSHAIR_YELLOW,
	CROSSHAIR_RED
};

class CursorManager
{
	public:
		static bool loadCursors();
		static void setCursor(sf::Window &window, CursorType type);
};
