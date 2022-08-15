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

/**
 * Loads the room data and stores it in this object.
 *
 * Room file structure:
 * {
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
 * @param roomFilePath path to room's json file
 * @param log pointer to Log object (need to figure out exceptions at some point...)
 * @returns `true` on load success, `false` otherwise
 */
bool Room::load(std::string roomFilePath)
{
	Json::Value root;

	if (!loadJsonFromFile(root, roomFilePath))
		return false;

	if (!this->loadArray(root, FOERR_JSON_KEY_BLOCKS, roomFilePath.c_str(), this->blocks))
		return false;

	if (!this->loadArray(root, FOERR_JSON_KEY_BACKGROUNDS, roomFilePath.c_str(), this->backgrounds))
		return false;

	return true;
}
