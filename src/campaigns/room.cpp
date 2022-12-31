#include "room.hpp"
#include <string>
#include <memory>
#include <SFML/Graphics/RenderTexture.hpp>
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
		this->backwall.setTextureRect({ 0, 0, GAME_AREA_WIDTH, GAME_AREA_HEIGHT });
		this->backwall.setColor(BACKWALL_COLOR);
	}

	// if liquid level is defined and > 0, room is fully submerged to that level (counting from bottom, in cells).
	// solids are also submerged
	this->liquidLevelHeight = 0;
	parseJsonKey<uint>(root, filePath, FOERR_JSON_KEY_LIQUID_LEVEL, this->liquidLevelHeight, true);
	if (this->liquidLevelHeight > 0)
	{
		std::string liquidSymbolStr;
		if (!parseJsonKey<std::string>(root, filePath, FOERR_JSON_KEY_LIQUID_SYMBOL, liquidSymbolStr))
			return false;

		if (liquidSymbolStr.length() != 1)
		{
			Log::e(STR_MAT_LOAD_KEY_NOT_1CHAR, liquidSymbolStr.c_str());
			return false;
		}

		char liquidSymbol = liquidSymbolStr[0];

		const struct material *liquidMat = matMgr.getOther(liquidSymbol);
		if (liquidMat == nullptr || liquidMat->type != MAT_LIQUID)
		{
			Log::e(STR_MAT_MISSING_OR_WRONG_TYPE, liquidSymbol);
			return false;
		}

		uint liquidLevelPx = CELL_SIDE_LEN * this->liquidLevelHeight;
		this->liquid.setSize(sf::Vector2f(GAME_AREA_WIDTH, liquidLevelPx));
		this->liquid.setPosition(0, GAME_AREA_HEIGHT - liquidLevelPx);
		this->liquid.setFillColor(liquidMat->color);
		this->liquidDelim.setTexture(resMgr.getTexture(liquidMat->textureDelimPath));
		this->liquidDelim.setColor(RoomCell::liquidSpriteColor);
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
				// cell needs to draw ladder or liquid delim based on the cell above it.
				// if y = 0 we don't want to draw delim
				bool topBlocksLadderDelim = y == 0 || this->cells[y - 1][x].blocksBottomCellLadderDelim();
				bool topBlocksLiquidDelim = y == 0 || this->cells[y - 1][x].blocksBottomCellLiquidDelim();

				if (symbol == ROOM_SYMBOL_UNKNOWN)
					Log::w(STR_UNKNOWN_SYMBOL_AT_POS, filePath.c_str(), FOERR_JSON_KEY_CELLS, x, y);
				else if (!this->cells[y][x].addOtherSymbol(symbol, topBlocksLadderDelim, topBlocksLiquidDelim, resMgr,
														   matMgr))
					return false;
			}
		}

		if (x != ROOM_WIDTH_WITH_BORDER - 1)
		{
			Log::e(STR_ROOM_ROW_TOO_SHORT, filePath.c_str(), FOERR_JSON_KEY_CELLS, y);
			return false;
		}

		for (auto &cell : this->cells[y])
		{
			if (!cell.finishSetup())
				return false;
		}
	}

	return true;
}

/**
 * Prepares the Room to be drawn.
 * Should be called *only once* per entering the Room. After that, use ::redrawCell() to update cells.
 */
void Room::init()
{
	sf::RenderTexture roomRenderTxt;
	roomRenderTxt.create(GAME_AREA_WIDTH, GAME_AREA_HEIGHT);
	roomRenderTxt.clear(sf::Color::Transparent);

	// TODO? calling the same nested loop three times is pretty lame, maybe find some better way to handle this

	for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			this->cells[y][x].draw1(roomRenderTxt);
		}
	}

	for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			this->cells[y][x].draw2(roomRenderTxt);
		}
	}

	for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			this->cells[y][x].draw3(roomRenderTxt);
		}
	}

	roomRenderTxt.display();

	// sf::RenderTexture can't be a private member because it inherits NonCopyable.
	// because of this we need to store the result in a standard sf::Texture
	this->cachedCellsTxt = roomRenderTxt.getTexture();
	this->cachedCells.setTexture(this->cachedCellsTxt);

	// we also need to pre-render liquid level. because of transparency and a sprite used for surface, the alpha will
	// get messed up if we simply draw it on top of liquid level rectangle. to counter this, we use sf::BlendNone.
	// but it would be difficult to use it along other elements (cells, backwall, etc.), therefore RenderTexture.
	// we can use the same RenderTexture which we used to draw cells as it's already the right size.
	roomRenderTxt.clear(sf::Color::Transparent);
	sf::RenderStates states(sf::BlendNone);

	// liquid level rectangle
	roomRenderTxt.draw(this->liquid, states);

	// delims (surface)
	if (this->liquidLevelHeight > 0 && this->liquidLevelHeight < ROOM_HEIGHT_WITH_BORDER)
	{
		// we only need to check the row above room-wide water level
		uint y = ROOM_HEIGHT_WITH_BORDER - this->liquidLevelHeight;
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			if (!this->cells[y - 1][x].blocksBottomCellLiquidDelim() && !this->cells[y][x].getHasSolid())
			{
				this->liquidDelim.setPosition(x * CELL_SIDE_LEN, y * CELL_SIDE_LEN);
				roomRenderTxt.draw(this->liquidDelim, states);
			}
		}
	}

	this->cachedLiquidLevelTxt = roomRenderTxt.getTexture();
	this->cachedLiquidLevel.setTexture(this->cachedLiquidLevelTxt);
}

/**
 * Clears cached textures used for rendering the Room.
 * Should be called when the Room is no longer displayed.
 */
void Room::deinit()
{
	this->cachedCellsTxt = sf::Texture();
	this->cachedLiquidLevelTxt = sf::Texture();
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
	cell->draw1(target);
	cell->draw2(target);
	cell->draw3(target);
}

void Room::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->backwall, states); // can be empty

	// because we render stuff first to RenderTexture, and then the texture to target, alpha gets blended two times,
	// and therefore gets screwed up. to fix it, use a custom blending mode.
	states.blendMode = BlendAlphaTransparent;
	target.draw(this->cachedCells, states);

	// liquid is drawn over all cell elements, including solids
	// liquids were also rendered to RenderTexture, but using sf::BlendNone, therefore the default blending mode works
	// correctly here.
	if (this->liquidLevelHeight > 0)
	{
		states.blendMode = sf::BlendAlpha;
		target.draw(this->cachedLiquidLevel, states);
	}
}
