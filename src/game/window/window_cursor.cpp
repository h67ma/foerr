#include <SFML/Window/Cursor.hpp>
#include <SFML/Graphics.hpp>
#include "window_cursor.hpp"
#include "../util/i18n.hpp"
#include "../consts.h"

WindowCursor::WindowCursor(sf::Window *window, Log *log)
{
	this->window = window;

	this->cursors[0].path = PATH_CURSOR_ARROW;
	this->cursors[0].hotX = 1;
	this->cursors[0].hotY = 1;
	this->cursors[0].fallbackCursor = sf::Cursor::Arrow;

	this->cursors[1].path = PATH_CURSOR_CROSS_WHITE;
	this->cursors[1].hotX = 1;
	this->cursors[1].hotY = 1;
	this->cursors[1].fallbackCursor = sf::Cursor::Cross;

	this->cursors[2].path = PATH_CURSOR_CROSS_YELLOW;
	this->cursors[2].hotX = 1;
	this->cursors[2].hotY = 1;
	this->cursors[2].fallbackCursor = sf::Cursor::Cross;

	this->cursors[3].path = PATH_CURSOR_CROSS_RED;
	this->cursors[3].hotX = 1;
	this->cursors[3].hotY = 1;
	this->cursors[3].fallbackCursor = sf::Cursor::Cross;

	for (uint i = 0; i < CURSORS_CNT; i++)
	{
		this->cursors[i].log = log;
	}
}

/**
 * Loads all cursors.
 *
 * @param preferCustom prefer loading custom images instead of system cursors.
 * @returns whether load succeeded.
 */
bool WindowCursor::loadCursors(bool preferCustom)
{
	bool succ = true;
	
	for (uint i = 0; i < CURSORS_CNT; i++)
	{
		succ = this->cursors[i].load(preferCustom);
		if (!succ)
			return false;
	}

	return true;
}

void WindowCursor::setCursor(CursorType type)
{
	this->window->setMouseCursor(this->cursors[type].cursor);
}
