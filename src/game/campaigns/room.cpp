#include <json/reader.h>
#include "room.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

bool Room::getDrawBackgroundFull()
{
	return this->drawBackgroundFull;
}

/**
 * Loads the room data and stores it in this object.
 *
 * Room file structure:
 * {
 *	"id": "room_id",
 *	"bg": false,	// optional
 *	"blocks": [
 *		"LD_|LD_|...",
 *		"Lfz|Lf_|...",
 *		"LD_|L_z|...",
 *		...
 *	]
 * }
 *
 * The block data is separated into 3-character "cells", separated by '|' character (for readability).
 * Each of the three letters corresponds to a different layer:
 *	- First letter defines the background
 *	- Second letter defines the background drawn on top of first background
 *	- Third letter defines the physical block
 * All three characters must be defined for every cell.
 * A special character '_' must be used to signal absence of block/background in a cell.
 * The dimensions of block grid are always 48x25 blocks (including "border" blocks).
 *
 * @param root reference to json node containing room data
 * @param filePath location file path, just for printing
 * @returns true on load success
 * @returns false on load fail
 */
bool Room::load(Json::Value &root, const char* filePath)
{
	// by default all rooms draw background full
	parseJsonBoolKey(root, filePath, FOERR_JSON_KEY_SHOW_ROOM_BG, this->drawBackgroundFull, true);

	if (!root.isMember(FOERR_JSON_KEY_BLOCKS))
	{
		Log::e(STR_MISSING_KEY, filePath, FOERR_JSON_KEY_BLOCKS);
		return false;
	}

	Json::Value node = root[FOERR_JSON_KEY_BLOCKS];
	if (!node.isArray())
	{
		Log::e(STR_INVALID_TYPE, filePath, FOERR_JSON_KEY_BLOCKS);
		return false;
	}

	if (node.size() < ROOM_HEIGHT_WITH_BORDER)
	{
		Log::e(STR_ROOM_MISSING_DATA, filePath, FOERR_JSON_KEY_BLOCKS);
		return false;
	}

	for (uint i = 0; i < ROOM_HEIGHT_WITH_BORDER; i++)
	{
		const char* line;

		try
		{
			line = node[i].asCString();
		}
		catch (const Json::LogicError &ex)
		{
			Log::e(STR_INVALID_TYPE_EX, filePath, FOERR_JSON_KEY_BLOCKS, ex.what());
			return false;
		}

		if (strlen(line) != ROOM_LINE_WIDTH_WITH_BORDER)
		{
			Log::e(STR_ROOM_LINE_INVALID, filePath, FOERR_JSON_KEY_BLOCKS, strlen(line), ROOM_LINE_WIDTH_WITH_BORDER);
			return false;
		}

		// finally, we can actually read the room data
		for (uint j = 0; j < ROOM_WIDTH_WITH_BORDER; j++)
		{
			// j is the "regular" x index here, i.e. it points to an element in blocks/backgrounds array, not in json line
			this->backgrounds[i][j] = line[j * ROOM_CHARS_PER_CELL];
			this->backgrounds2[i][j] = line[(j * ROOM_CHARS_PER_CELL) + 1];
			this->blocks[i][j] = line[(j * ROOM_CHARS_PER_CELL) + 2];
			// line[(j * ROOM_CHARS_PER_CELL) + 3] should be '|', but we don't actually need to check it, some character just needs to be here.
			// actually, because we skip the '|' at the end of the line, the character at the end would be 0.
		}
	}

	return true;
}

void Room::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	// TODO
}
