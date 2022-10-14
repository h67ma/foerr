#include <SFML/Window/Cursor.hpp>
#include <SFML/Graphics.hpp>
#include "cursor_manager.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"
#include "../consts.hpp"

/**
 * Loads all cursors.
 *
 * @param preferCustom prefer loading custom images instead of system cursors.
 * @returns whether load succeeded.
 */
bool CursorManager::loadCursors(bool preferCustom)
{
	for (uint i = 0; i < _CURSORS_CNT; i++)
	{
		if (!this->cursors[i].load(preferCustom))
			return false;
	}

	return true;
}

void CursorManager::setCursor(sf::RenderWindow &window, CursorType type)
{
	if (type >= _CURSORS_CNT)
	{
		Log::w(STR_IDX_OUTTA_BOUNDS);
		return;
	}

	window.setMouseCursor(this->cursors[type].getCursor());
}
