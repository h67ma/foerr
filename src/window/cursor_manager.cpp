#include "cursor_manager.hpp"
#include <string>
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

CursorManager::CursorManager()
{
	// this is a pretty annoying workaround for sf::Cursor being non-copyable.
	// TODO? is there even a way to make it a bit nicer?
	this->cursors.emplace(POINTER, std::make_unique<CustomCursor>(std::string(PATH_CURSOR_ARROW), sf::Vector2u(1, 1), sf::Cursor::Arrow));
	this->cursors.emplace(CROSSHAIR_WHITE, std::make_unique<CustomCursor>(std::string(PATH_CURSOR_CROSS_WHITE), sf::Vector2u(13, 13), sf::Cursor::Cross));
	this->cursors.emplace(CROSSHAIR_YELLOW, std::make_unique<CustomCursor>(std::string(PATH_CURSOR_CROSS_YELLOW), sf::Vector2u(13, 13), sf::Cursor::Cross));
	this->cursors.emplace(CROSSHAIR_RED, std::make_unique<CustomCursor>(std::string(PATH_CURSOR_CROSS_RED), sf::Vector2u(13, 13), sf::Cursor::Cross));
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
		if (!cursor.second->load())
			return false;
	}

	return true;
}

void CursorManager::setCursor(sf::RenderWindow &window, CursorType type)
{
	auto search = this->cursors.find(type);
	if (search == this->cursors.end())
	{
		Log::w(STR_IDX_OUTTA_BOUNDS);
		return;
	}

	window.setMouseCursor(search->second->getCursor());
}
