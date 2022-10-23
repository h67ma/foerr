#pragma once

#include <unordered_map>
#include <memory>
#include <SFML/Window/Cursor.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
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
		std::unordered_map<CursorType, std::unique_ptr<CustomCursor>> cursors;

	public:
		CursorManager();
		bool loadCursors(bool preferCustom);
		void setCursor(sf::RenderWindow &window, CursorType type);
};
