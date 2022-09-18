#include <json/reader.h>
#include "room.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

bool Room::loadArray(Json::Value &root, const char* key, const char* filename, char (*arr)[ROOM_WIDTH_WITH_BORDER])
{
	if (!root.isMember(key))
	{
		Log::e(STR_MISSING_KEY, filename, key);
		return false;
	}

	Json::Value node = root[key];
	if (!node.isArray())
	{
		Log::e(STR_INVALID_TYPE, filename, key);
		return false;
	}

	if (node.size() < ROOM_HEIGHT_WITH_BORDER)
	{
		Log::e(STR_ROOM_MISSING_DATA, filename, key);
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
			Log::e(STR_INVALID_TYPE_EX, filename, key, ex.what());
			return false;
		}

		if (strlen(line) < ROOM_WIDTH_WITH_BORDER)
		{
			Log::e(STR_ROOM_MISSING_DATA, filename, key);
			return false;
		}

		// finally, we can actually read the room data
		// could also use strncpy_s if it weren't for the final null char
		for (uint j = 0; j < ROOM_WIDTH_WITH_BORDER; j++)
		{
			arr[i][j] = line[j];
		}
	}

	return true;
}

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
 *		"DDDDDDD...",
 *		"Dnnnnnn...",
 *		"Dnnpppp...",
 *		...
 *	],
 *	"backgrounds": [
 *		"LLLLLLL...",
 *		"LLLLLLL...",
 *		"fffffff...",
 *		...
 *	]
 * }
 *
 * @param root json node containing room data
 * @param filePath location file path, just for printing
 * @returns true on load success
 * @returns false on load fail
 */
bool Room::load(Json::Value &root, const char* filePath)
{
	// by default all rooms draw background full
	parseJsonBoolKey(root, filePath, FOERR_JSON_KEY_SHOW_ROOM_BG, this->drawBackgroundFull, true);

	if (!this->loadArray(root, FOERR_JSON_KEY_BLOCKS, filePath, this->blocks))
		return false;

	if (!this->loadArray(root, FOERR_JSON_KEY_BACKGROUNDS, filePath, this->backgrounds))
		return false;

	return true;
}

void Room::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	// TODO
}
