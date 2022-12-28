#include "room.hpp"
#include <string>
#include <memory>
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

#define ROOM_SYMBOL_SEPARATOR '|'
#define ROOM_SYMBOL_EMPTY '_'
#define ROOM_SYMBOL_UNKNOWN '?'

/**
 * Loads the room data and stores it in this object.
 *
 * Room node structure:
 * {
 *	"coords": [2, 0, 1],	// (X, Y, Z). Z=0 is the front/main layer, Z=1 is backstage-1, Z=2 is backstage-2, etc.
 *	"is_start": true,		// optional, exactly one room in a location must be a start room
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
 * To overwrite full background, just define a backwall for the room.
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
bool Room::load(ResourceManager &resMgr, const MaterialManager &matMgr, const json &root, const std::string &filePath)
{
	// backwall can be empty
	std::string backwallTxtPath = "";
	parseJsonKey<std::string>(root, filePath, FOERR_JSON_KEY_BACKWALL, backwallTxtPath, true);
	if (backwallTxtPath != "")
	{
		backwallTxtPath = pathCombine(PATH_TEXT_CELLS, backwallTxtPath + ".png");
		std::shared_ptr<sf::Texture> backwallTxt = resMgr.getTexture(backwallTxtPath);
		backwallTxt->setRepeated(true);
		this->backwall.setTexture(backwallTxt);
		this->backwall.get().setTextureRect({ 0, 0, GAME_AREA_WIDTH, GAME_AREA_HEIGHT });
		this->backwall.get().setColor(BACKWALL_COLOR);
	}

	// if liquid level is defined and > 0, room is fully submerged to that level (counting from bottom, in cells).
	// solids are also submerged
	uint liquidLevel = 0;
	parseJsonKey<uint>(root, filePath, FOERR_JSON_KEY_LIQUID_LEVEL, liquidLevel, true);
	if (liquidLevel > 0)
	{
		uint liquidLevelPx = CELL_SIDE_LEN * liquidLevel;
		this->liquid.setSize(sf::Vector2f(GAME_AREA_WIDTH, liquidLevelPx));
		this->liquid.setPosition(0, GAME_AREA_HEIGHT - liquidLevelPx);
		this->liquid.setFillColor(sf::Color(30, 110, 190, 154)); // TODO set proper color
	}

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
				// first symbol (solid)
				firstCellSymbol = false;

				// the cell needs to know its position to properly set texture rect, and to draw itself in correct place
				this->cells[y][x].setPosition(x * CELL_SIDE_LEN, y * CELL_SIDE_LEN);

				if (symbol == ROOM_SYMBOL_UNKNOWN)
				{
					Log::w(STR_UNKNOWN_SYMBOL_AT_POS, filePath.c_str(), FOERR_JSON_KEY_CELLS, x, y);
				}
				else if (symbol != ROOM_SYMBOL_EMPTY)
				{
					if (!this->cells[y][x].addSolidSymbol(symbol, resMgr, matMgr))
						return false;
				}
			}
			else if (symbol != ROOM_SYMBOL_EMPTY)
			{
				// 2nd, 3rd, etc. symbol, not empty

				// because we load cells Frgt/10 style (from the top to the bottom), we already know if a particular
				// cell needs to draw ladder delim based on the cell above it.
				// if y = 0 we don't want to draw delim
				bool topBlocksLadderDelim = y == 0 || this->cells[y - 1][x].blocksBottomCellLadderDelim();

				if (symbol == ROOM_SYMBOL_UNKNOWN)
					Log::w(STR_UNKNOWN_SYMBOL_AT_POS, filePath.c_str(), FOERR_JSON_KEY_CELLS, x, y);
				else if (!this->cells[y][x].addOtherSymbol(symbol, topBlocksLadderDelim, resMgr, matMgr))
					return false;
			}
		}

		if (x != ROOM_WIDTH_WITH_BORDER - 1)
		{
			Log::e(STR_ROOM_ROW_TOO_SHORT, filePath.c_str(), FOERR_JSON_KEY_CELLS, y);
			return false;
		}

		for (const auto &cell : this->cells[y])
		{
			if (!cell.validate())
				return false;
		}
	}

	return true;
}

/**
 * @brief Redraws a single cell. For drawing the entire Room, use ::draw()
 *
 * If cell coordinates are invalid, nothing will happen.
 *
 * @param x cell x coordinate
 * @param y cell y coordinate
 * @param target reference to render target
 * @param states render states
 */
void Room::redrawCell(uint x, uint y, sf::RenderTarget &target, sf::RenderStates states) const
{
	if (x >= ROOM_WIDTH_WITH_BORDER || y >= ROOM_HEIGHT_WITH_BORDER)
		return;

	const RoomCell *cell = &this->cells[y][x];
	cell->draw1(target, states);
	cell->draw2(target, states);
	cell->draw3(target, states);
}

/**
 * @brief Draws all cells in the Room.
 *
 * Should be called *only once* per entering the Room. After that, use ::redrawCell() to update cells.
 */
void Room::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->backwall.sprite, states); // can be empty

	// TODO? calling the same nested loop three times is pretty lame, maybe find some better way to handle this

	for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			this->cells[y][x].draw1(target, states);
		}
	}

	for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			this->cells[y][x].draw2(target, states);
		}
	}

	for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			this->cells[y][x].draw3(target, states);
		}
	}

	// liquid is drawn over all cell elements, including solids. if it's not set, nothing will be drawn.
	target.draw(this->liquid, states);
}
