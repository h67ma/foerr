#include "room.hpp"
#include <string>
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

bool Room::shouldDrawBackgroundFull()
{
	return this->drawBackgroundFull;
}

/**
 * Loads the room data and stores it in this object.
 *
 * Room file structure:
 * {
 *	"coords": [2, 0, 1],	// (X, Y, Z). Z=0 is the front/main layer, Z=1 is backstage-1, Z=2 is backstage-2, etc.
 *	"is_start": true,		// optional, exactly one room in a location must be a start room
 *	"bg": false,			// optional, false means that background_full is not displayed for this room even if
 *							// the location specifies it (useful for e.g. underground or backstage rooms)
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

	auto blocksSearch = root.find(FOERR_JSON_KEY_BLOCKS);
	if (blocksSearch == root.end())
	{
		Log::e(STR_MISSING_KEY, filePath.c_str(), FOERR_JSON_KEY_BLOCKS);
		return false;
	}

	if (!blocksSearch->is_array())
	{
		Log::e(STR_INVALID_TYPE, filePath.c_str(), FOERR_JSON_KEY_BLOCKS);
		return false;
	}

	if (blocksSearch->size() != ROOM_HEIGHT_WITH_BORDER)
	{
		Log::e(STR_INVALID_ARR_SIZE, filePath.c_str(), FOERR_JSON_KEY_BLOCKS);
		return false;
	}

	for (auto it = blocksSearch->begin(); it != blocksSearch->end(); it++)
	{
		std::string line;

		try
		{
			line = (*it);
		}
		catch (const json::type_error &ex)
		{
			Log::e(STR_INVALID_TYPE_EX, filePath.c_str(), FOERR_JSON_KEY_BLOCKS, ex.what());
			return false;
		}

		if (line.length() != ROOM_LINE_WIDTH_WITH_BORDER)
		{
			Log::e(STR_ROOM_LINE_INVALID, filePath.c_str(), FOERR_JSON_KEY_BLOCKS, line.length(), ROOM_LINE_WIDTH_WITH_BORDER);
			return false;
		}

		uint y = static_cast<uint>(std::distance(blocksSearch->begin(), it));

		// finally, we can actually read the room data
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			// x is the "regular" horizontal index here, i.e. it points to an element in blocks/backgrounds array,
			// not in json line
			this->backgrounds[y][x] = line[x * ROOM_CHARS_PER_CELL];
			this->backgrounds2[y][x] = line[(x * ROOM_CHARS_PER_CELL) + 1];
			this->blocks[y][x] = line[(x * ROOM_CHARS_PER_CELL) + 2];
			// line[(x * ROOM_CHARS_PER_CELL) + 3] should be '|', but we don't actually need to check it.
			// some character just needs to be here.
			// actually, because we skip the '|' at the end of the line, the character at the end would be 0.
		}
	}

	return true;
}

void Room::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	// TODO
}
