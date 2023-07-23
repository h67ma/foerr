#pragma once

#include <unordered_map>

#include <SFML/Window/Window.hpp>

#include "custom_cursor.hpp"

enum CursorType {
	POINTER,
	CROSSHAIR_WHITE,
	CROSSHAIR_YELLOW,
	CROSSHAIR_RED
};

class CursorManager
{
	private:
		std::unordered_map<CursorType, CustomCursor> cursors;
		sf::Window &window;

	public:
		explicit CursorManager(sf::Window &window);
		bool loadCursors();
		void setCursor(CursorType type);
};
