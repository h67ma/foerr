#include "room.hpp"
#include <string>
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

#define ROOM_SYMBOL_SEPARATOR '|'
#define ROOM_SYMBOL_EMPTY '_'

bool Room::shouldDrawBackgroundFull()
{
	return this->drawBackgroundFull;
}

/**
 * Loads the room data and stores it in this object.
 *
 * Room node structure:
 * {
 *	"coords": [2, 0, 1],	// (X, Y, Z). Z=0 is the front/main layer, Z=1 is backstage-1, Z=2 is backstage-2, etc.
 *	"is_start": true,		// optional, exactly one room in a location must be a start room
 *	"bg": false,			// optional, false means that background_full is not displayed for this room even if
 *							// the location specifies it (useful for e.g. underground or backstage rooms)
 *	"backwall": "tName",	// optional, name of texture which should be displayed where no cell background is defined
 *	"cells": [
 *		"H|_-|...",
 *		"_Ka|G;|...",
 *		...
 *	]
 * }
 *
 * The cell data is separated by ROOM_SYMBOL_SEPARATOR ('_'). Each cell is of 1 or more characters in length.
 * First character in a cell corresponds to a solid, and can be empty ('_').
 * Rest of the symbols define various elements, such as background, stairs, ladder, water.
 * The dimensions of cell grid are always 48x25 cells (including border cells).
 *
 * Note: while it would be the most efficient (both in storage and in processing time) to store room data in binary
 * format, this approach has downsides. First, text is easily editable, which allows casual hackers to experiment with
 * modifying rooms using only a text editor. It also makes it much easier to develop and debug the game, or tools used
 * for creating levels. Secondly, it makes it possible to track changes in data using version control, which is useful
 * for keeping file history, blaming, etc. This applies to all data files, but rooms are the most obvious candidate for
 * binarization as they are the largest in volume.
 *
 * @param root reference to json node containing room data
 * @param filePath location file path, just for printing
 * @returns true on load success
 * @returns false on load fail
 */
bool Room::load(const json &root, const std::string &filePath)
{
	// by default all rooms draw background full
	parseJsonKey<bool>(root, filePath, FOERR_JSON_KEY_SHOW_ROOM_BG, this->drawBackgroundFull, true);

	// TODO load and use backwall

	auto cellsSearch = root.find(FOERR_JSON_KEY_CELLS);
	if (cellsSearch == root.end())
	{
		Log::e(STR_MISSING_KEY, filePath.c_str(), FOERR_JSON_KEY_CELLS);
		return false;
	}

	if (!cellsSearch->is_array())
	{
		Log::e(STR_INVALID_TYPE, filePath.c_str(), FOERR_JSON_KEY_CELLS);
		return false;
	}

	if (cellsSearch->size() != ROOM_HEIGHT_WITH_BORDER)
	{
		Log::e(STR_INVALID_ARR_SIZE, filePath.c_str(), FOERR_JSON_KEY_CELLS);
		return false;
	}

	for (auto it = cellsSearch->begin(); it != cellsSearch->end(); it++)
	{
		std::string line;

		try
		{
			line = (*it);
		}
		catch (const json::type_error &ex)
		{
			Log::e(STR_INVALID_TYPE_EX, filePath.c_str(), FOERR_JSON_KEY_CELLS, ex.what());
			return false;
		}

		uint y = static_cast<uint>(std::distance(cellsSearch->begin(), it));
		uint x = 0;

		bool firstCellSymbol = true;
		for (const auto symbol : line)
		{
			if (symbol == ROOM_SYMBOL_SEPARATOR)
			{
				x++;
				firstCellSymbol = true;
				continue;
			}

			if (x >= ROOM_WIDTH_WITH_BORDER)
			{
				Log::w(STR_ROOM_LINE_TOO_LONG, filePath.c_str(), FOERR_JSON_KEY_CELLS, ROOM_WIDTH_WITH_BORDER);
				break;
			}

			if (firstCellSymbol)
			{
				firstCellSymbol = false;
				if (symbol != ROOM_SYMBOL_EMPTY)
				{
					if (!this->cells[y][x].addSolidSymbol(symbol))
						return false;
				}
			}
			else if (symbol != ROOM_SYMBOL_EMPTY)
			{
				if (!this->cells[y][x].addOtherSymbol(symbol))
					return false;
			}
		}
	}

	return true;
}

void Room::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	// TODO
}
