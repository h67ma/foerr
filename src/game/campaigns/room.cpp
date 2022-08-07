#include <fstream>
#include <json/reader.h>
#include "room.hpp"
#include "../util/i18n.hpp"

bool Room::loadArray(Json::Value &root, const char* key, const char* filename, char (*arr)[ROOM_WIDTH_WITH_BORDER], Log &log)
{
	if (!root.isMember(key))
	{
		log.log(LOG_ERROR, STR_MISSING_KEY, filename, key);
		return false;
	}

	Json::Value node = root[key];
	if (!node.isArray())
	{
		log.log(LOG_ERROR, STR_INVALID_TYPE, filename, key);
		return false;
	}

	if (node.size() < ROOM_HEIGHT_WITH_BORDER)
	{
		log.log(LOG_ERROR, STR_ROOM_MISSING_DATA, filename, key);
		return false;
	}

	for (uint i = 0; i < ROOM_HEIGHT_WITH_BORDER; i++)
	{
		const char* line;

		try
		{
			// no idea how to use asString(), access violation and no explanation.
			// empty documentation also doesn't help.
			// I don't give a damn about bad reputation, need to get some C++ education.
			line = node[i].asCString();
		}
		catch (const Json::LogicError& ex)
		{
			log.log(LOG_ERROR, STR_INVALID_TYPE_EX, filename, key, ex.what());
			return false;
		}

		if (strlen(line) < ROOM_WIDTH_WITH_BORDER)
		{
			log.log(LOG_ERROR, STR_ROOM_MISSING_DATA, filename, key);
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
bool Room::load(std::string roomFilePath, Log &log)
{
	std::ifstream file(roomFilePath, std::ifstream::binary);
	Json::Value root;

	if (!file.is_open())
	{
		log.log(LOG_ERROR, STR_FILE_OPEN_ERROR, roomFilePath.c_str());
		return false;
	}

	try
	{
		file >> root;
	}
	catch (const Json::RuntimeError& ex)
	{
		log.log(LOG_ERROR, STR_SYNTAX_ERROR, roomFilePath.c_str(), ex.what());
		file.close(); // no finally :(
		return false;
	}

	file.close();

	this->loadArray(root, FOERR_JSON_KEY_ROOM_BLOCKS, roomFilePath.c_str(), this->blocks, log);
	this->loadArray(root, FOERR_JSON_KEY_ROOM_BACKGROUNDS, roomFilePath.c_str(), this->backgrounds, log);

	return true;
}
