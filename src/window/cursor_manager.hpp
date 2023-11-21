// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

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
		sf::Window& window;

	public:
		explicit CursorManager(sf::Window& window);
		bool loadCursors();
		void setCursor(CursorType type);
};
