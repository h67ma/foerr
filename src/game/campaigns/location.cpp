#include <json/reader.h>
#include "location.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

/**
 * Loads location details, "big" background, and all rooms located
 * in `locDir` (json files, each one contains a single room).
 *
 * Location file structure:
 * {
 *	"grind": false,
 *	"basecamp": false,
 *	"background_full": "path/to/img.png",
 *	"width": 2,
 *	"height": 3,
 *	"room_map": [
 *		["one",		"two"],
 *		["three",	"/EMPTY"],
 *		["five",	"six"]
 *	]
 * }
 *
 * "room_map" contains room ids - room files should be located in the same directory
 * as index file and called the same as their ids, with ".json" extension.
 * Rooms are stored in natural order, i.e. rows represent horizontal strips,
 * columns represent vertical strips.
 * The special value "/EMPTY" will result in an empty room (nullptr) added to the room grid.
 *
 * In case when loading fails, all previously allocated resources (i.e. rooms) will be
 * automatically deallocated.
 *
 * @param locDir path to this location's directory, containing index file and room files
 * @param resMgr reference to resource manager object
 * @returns `true` if load succeeded, `false` otherwise.
 */
bool Location::load(std::string locDir, ResourceManager& resMgr)
{
	std::string indexPath = pathCombine(locDir, std::string(FILENAME_INDEX));
	std::string backgroundFullPath;
	uint width, height;
	Json::Value root;

	if (!loadJsonFromFile(root, indexPath))
		return false;

	if (!parseJsonStringKey(root, indexPath.c_str(), FOERR_JSON_KEY_TITLE, this->title))
		return false;

	if (!parseJsonStringKey(root, indexPath.c_str(), FOERR_JSON_KEY_DESCRIPTION, this->description))
		return false;

	// TODO translate title & description

	if (!parseJsonBoolKey(root, indexPath.c_str(), FOERR_JSON_KEY_TYPE_GRIND, this->isGrind))
		return false;

	if (!parseJsonBoolKey(root, indexPath.c_str(), FOERR_JSON_KEY_TYPE_BASECAMP, this->isBasecamp))
		return false;

	if (this->isBasecamp && this->isGrind)
	{
		// something stinks here...
		Log::e(STR_LOC_INVALID_TYPES, indexPath.c_str());
		return false;
	}

	if (!parseJsonStringKey(root, indexPath.c_str(), FOERR_JSON_KEY_BACKGROUND_FULL, backgroundFullPath))
		return false;

	sf::Texture *backgroundFull = resMgr.loadTexture(backgroundFullPath);
	if (backgroundFull == nullptr)
		return false;

	this->backgroundFullSprite.setTexture(*backgroundFull);

	if (!parseJsonUintKey(root, indexPath.c_str(), FOERR_JSON_KEY_WIDTH, width))
		return false;

	if (!parseJsonUintKey(root, indexPath.c_str(), FOERR_JSON_KEY_HEIGHT, height))
		return false;

	this->rooms.setDimens(width, height);

	if (!root.isMember(FOERR_JSON_KEY_ROOM_MAP))
	{
		Log::e(STR_MISSING_KEY, indexPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
		return false;
	}

	// TODO this is relevant for unique locations - put it in function or inherit classes for unique and grind
	Json::Value roomMap = root[FOERR_JSON_KEY_ROOM_MAP];

	if (!roomMap.isArray())
	{
		Log::e(STR_INVALID_TYPE, indexPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
		return false;
	}

	if (roomMap.size() < height)
	{
		Log::e(STR_LOC_MISSING_DATA, indexPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
		return false;
	}

	for (uint y = 0; y < height; y++)
	{
		Json::Value roomMapRow = roomMap[y];
		if (!roomMapRow.isArray())
		{
			Log::e(STR_INVALID_TYPE, indexPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
			return false;
		}

		if (roomMapRow.size() < width)
		{
			Log::e(STR_LOC_MISSING_DATA, indexPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
			return false;
		}

		// all good, can now read room ids
		for (uint x = 0; x < width; x++)
		{
			std::string roomName;
			try
			{
				roomName = std::string(roomMapRow[x].asCString());
			}
			catch (const Json::LogicError &ex)
			{
				Log::e(STR_INVALID_TYPE_EX, indexPath.c_str(), FOERR_JSON_KEY_ROOM_MAP, ex.what());
				return false;
			}

			Room *room = nullptr;
			if (roomName != ROOM_EMPTY)
			{
				std::string roomPath = pathCombine(locDir, roomName) + EXT_JSON;
				room = new Room();
				if (!room->load(roomPath))
				{
					delete room;
					this->rooms.clear();
					// TODO also clear location-specific resources loaded in resMgr
					return false;
				}
			}

			if (!this->rooms.set(x, y, room))
			{
				// this can't really fail, but better safe than sorry

				if (room != nullptr)
					delete room;

				this->rooms.clear();
				// TODO also clear location-specific resources loaded in resMgr
				return false;
			}
		}
	}

	Log::d(STR_LOADED_LOCATION, indexPath.c_str());
	return true;
}

std::string Location::getTitle()
{
	return this->title;
}

std::string Location::getDescription()
{
	return this->description;
}

void Location::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(this->backgroundFullSprite, states);

	// TODO draw current room
}
