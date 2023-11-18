// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "cursor_manager.hpp"

#include <SFML/Window/Cursor.hpp>

#include "../util/i18n.hpp"
#include "../hud/log.hpp"

CursorManager::CursorManager(sf::Window &window) : window(window)
{
	this->cursors.try_emplace(POINTER, PATH_CURSOR_ARROW, sf::Vector2u(1, 1), sf::Cursor::Arrow);
	this->cursors.try_emplace(CROSSHAIR_WHITE, PATH_CURSOR_CROSS_WHITE, sf::Vector2u(13, 13), sf::Cursor::Cross);
	this->cursors.try_emplace(CROSSHAIR_YELLOW, PATH_CURSOR_CROSS_YELLOW, sf::Vector2u(13, 13), sf::Cursor::Cross);
	this->cursors.try_emplace(CROSSHAIR_RED, PATH_CURSOR_CROSS_RED, sf::Vector2u(13, 13), sf::Cursor::Cross);
}

/**
 * Loads all cursors.
 *
 * @param preferCustom prefer loading custom images instead of system cursors.
 * @returns whether load succeeded.
 */
bool CursorManager::loadCursors()
{
	for (auto &cursor : this->cursors)
	{
		if (!cursor.second.load())
			return false;
	}

	return true;
}

void CursorManager::setCursor(CursorType type)
{
	auto search = this->cursors.find(type);
	if (search == this->cursors.end())
	{
		Log::w(STR_CURSOR_SET_FAIL);
		return;
	}

	this->window.setMouseCursor(search->second);
}
