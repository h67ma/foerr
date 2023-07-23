#include "cursor_manager.hpp"

#include <SFML/Window/Cursor.hpp>

#include <unordered_map>
#include <string>

#include "custom_cursor.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

std::unordered_map<CursorType, CustomCursor> cursors;

/**
 * Loads all cursors.
 *
 * @param preferCustom prefer loading custom images instead of system cursors.
 * @returns whether load succeeded.
 */
bool CursorManager::loadCursors()
{
	cursors.try_emplace(POINTER, PATH_CURSOR_ARROW, sf::Vector2u(1, 1), sf::Cursor::Arrow);
	cursors.try_emplace(CROSSHAIR_WHITE, PATH_CURSOR_CROSS_WHITE, sf::Vector2u(13, 13), sf::Cursor::Cross);
	cursors.try_emplace(CROSSHAIR_YELLOW, PATH_CURSOR_CROSS_YELLOW, sf::Vector2u(13, 13), sf::Cursor::Cross);
	cursors.try_emplace(CROSSHAIR_RED, PATH_CURSOR_CROSS_RED, sf::Vector2u(13, 13), sf::Cursor::Cross);

	for (auto &cursor : cursors)
	{
		if (!cursor.second.load())
			return false;
	}

	return true;
}

void CursorManager::setCursor(sf::RenderWindow &window, CursorType type)
{
	auto search = cursors.find(type);
	if (search == cursors.end())
	{
		Log::w(STR_IDX_OUTTA_BOUNDS);
		return;
	}

	window.setMouseCursor(search->second);
}
