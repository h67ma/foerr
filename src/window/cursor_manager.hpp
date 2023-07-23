#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

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
		static void setCursor(sf::RenderWindow &window, CursorType type);
};
