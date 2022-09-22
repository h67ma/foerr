#include <json/reader.h>
#include "location.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

#define LOC_MAX_DIMEN 100 // max number of rooms horizontally/vertically
#define LOC_WORLDMAP_MAX 600 // max x/y coordinate of worldmap icons

Location::Location(std::string id, std::string locPath) : id(id), locPath(locPath)
{
	// "It's ghouls, I tell ya. Religious ghouls in rockets looking for a land to call their own."
}

/**
 * Loads location details, mainly for display on the world map.
 * Rooms and background big are loaded separately via ::loadContent().
 *
 * @returns true if load succeeded
 * @returns false if load failed
 */
bool Location::loadMeta()
{
	Json::Value root;

	Log::d(STR_LOADING_LOCATION_META, this->locPath.c_str());

	if (!loadJsonFromFile(root, this->locPath))
		return false;

	if (!parseJsonStringKey(root, this->locPath.c_str(), FOERR_JSON_KEY_TITLE, this->title))
		return false;

	if (!parseJsonStringKey(root, this->locPath.c_str(), FOERR_JSON_KEY_DESCRIPTION, this->description))
		return false;

	// TODO translate title & description

	if (!parseJsonBoolKey(root, this->locPath.c_str(), FOERR_JSON_KEY_TYPE_GRIND, this->isGrind))
		return false;

	if (!parseJsonBoolKey(root, this->locPath.c_str(), FOERR_JSON_KEY_TYPE_BASECAMP, this->isBasecamp))
		return false;

	if (this->isBasecamp && this->isGrind)
	{
		// something stinks here...
		Log::e(STR_LOC_INVALID_TYPES, this->locPath.c_str());
		return false;
	}

	parseJsonUintKey(root, this->locPath.c_str(), FOERR_JSON_KEY_RECOMMENDED_LVL, this->recommendedLevel, true);

	if (!parseJsonStringKey(root, this->locPath.c_str(), FOERR_JSON_KEY_WORLDMAP_ICON, this->worldMapIconId))
		return false;

	if (!parseJsonVector2uKey(root, this->locPath.c_str(), FOERR_JSON_KEY_WORLDMAP_COORDS, this->worldMapCoords))
		return false;

	if (this->worldMapCoords.x > LOC_WORLDMAP_MAX || this->worldMapCoords.y > LOC_WORLDMAP_MAX)
	{
		Log::e(STR_SUS_LARGE_VALUE, FOERR_JSON_KEY_WORLDMAP_COORDS);
		return false;
	}

	// not present -> default value (false)
	parseJsonBoolKey(root, this->locPath.c_str(), FOERR_JSON_KEY_WORLDMAP_ICON_BIG, this->isWorldMapIconBig, true);

	Log::d(STR_LOADED_LOCATION_META, this->locPath.c_str());
	return true;
}

/**
 * Loads room data and big background.
 *
 * "room_map" contains room ids. Rooms are stored in natural order, i.e. rows represent
 * horizontal strips, columns represent vertical strips.
 * The special value ROOM_EMPTY will result in an empty room (nullptr) added to the room grid.
 *
 * In case when loading fails, all previously allocated rooms will be automatically deallocated.
 *
 * @param resMgr reference to resource manager object
 * @returns true if load succeeded
 * @returns false if load failed
 */
bool Location::loadContent(ResourceManager &resMgr)
{
	std::string backgroundFullPath;
	Json::Value root;

	this->unloadContent();

	Log::d(STR_LOADING_LOCATION_CONTENT, this->locPath.c_str());

	if (!loadJsonFromFile(root, this->locPath))
		return false;

	// not present -> black background
	if (parseJsonStringKey(root, this->locPath.c_str(), FOERR_JSON_KEY_BACKGROUND_FULL, backgroundFullPath, true))
	{
		std::shared_ptr<sf::Texture> backgroundFull = resMgr.getTexture(backgroundFullPath);
		if (backgroundFull == nullptr)
			return false;

		this->backgroundFullSprite.setTexture(backgroundFull);
	}

	if (!root.isMember(FOERR_JSON_KEY_ROOMS))
	{
		Log::e(STR_MISSING_KEY, this->locPath.c_str(), FOERR_JSON_KEY_ROOMS);
		return false;
	}

	// note: jsoncpp can't do object iteration, and getMemberNames() is straight up broken.
	// so for now we store it the stupidass way, as array with member keys :/
	// {
	//	"id": "room_id",
	//	...
	// }

	Json::Value roomDefs = root[FOERR_JSON_KEY_ROOMS];
	if (!roomDefs.isArray())
	{
		Log::e(STR_INVALID_TYPE, this->locPath.c_str(), FOERR_JSON_KEY_ROOMS);
		return false;
	}

	// first load all room data into dictionary, mapping room id -> Room
	// there are no duplicates in the dictionary (duh), but they can happen in room grid
	std::unordered_map<std::string, std::shared_ptr<Room>> roomDict;

	for (uint i = 0; i < roomDefs.size(); i++)
	{
		// this is utterly retarded
		std::string roomId;
		if (!parseJsonStringKey(roomDefs[i], this->locPath.c_str(), FOERR_JSON_KEY_ID, roomId))
			return false;

		auto duplicate = roomDict.find(roomId);
		if (duplicate != roomDict.end())
		{
			Log::e(STR_ROOM_DUPLICATE, this->locPath.c_str(), roomId.c_str());
			return false;
		}

		std::shared_ptr<Room> room = std::make_shared<Room>();
		if (!room->load(roomDefs[i], this->locPath.c_str()))
			return false;

		roomDict[roomId] = room;
	}

	// room data loaded, now arrange rooms in a grid, and this is how we'll store them in the end

	// initial player room coords
	parseJsonVector2uKey(root, this->locPath.c_str(), FOERR_JSON_KEY_START_COORDS, this->playerRoomCoords, true);

	sf::Vector2u gridDimens;
	if (!parseJsonVector2uKey(root, this->locPath.c_str(), FOERR_JSON_KEY_DIMENS, gridDimens))
		return false;

	if (gridDimens.x > LOC_MAX_DIMEN || gridDimens.y > LOC_MAX_DIMEN)
	{
		Log::e(STR_SUS_LARGE_VALUE, FOERR_JSON_KEY_DIMENS);
		return false;
	}

	this->rooms.setDimens(gridDimens);

	if (!root.isMember(FOERR_JSON_KEY_ROOM_MAP))
	{
		Log::e(STR_MISSING_KEY, this->locPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
		this->unloadContent();
		return false;
	}

	Json::Value roomMap = root[FOERR_JSON_KEY_ROOM_MAP];
	if (!roomMap.isArray())
	{
		Log::e(STR_INVALID_TYPE, this->locPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
		this->unloadContent();
		return false;
	}

	if (roomMap.size() < gridDimens.y)
	{
		Log::e(STR_LOC_MISSING_DATA, this->locPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
		this->unloadContent();
		return false;
	}

	for (uint y = 0; y < gridDimens.y; y++)
	{
		Json::Value roomMapRow = roomMap[y];
		if (!roomMapRow.isArray())
		{
			Log::e(STR_INVALID_TYPE, this->locPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
			this->unloadContent();
			return false;
		}

		if (roomMapRow.size() < gridDimens.x)
		{
			Log::e(STR_LOC_MISSING_DATA, this->locPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
			this->unloadContent();
			return false;
		}

		// all good, can now read room ids
		for (uint x = 0; x < gridDimens.x; x++)
		{
			std::string roomName;
			try
			{
				roomName = std::string(roomMapRow[x].asCString());
			}
			catch (const Json::LogicError &ex)
			{
				Log::e(STR_INVALID_TYPE_EX, this->locPath.c_str(), FOERR_JSON_KEY_ROOM_MAP, ex.what());
				this->unloadContent();
				return false;
			}

			if (roomName == ROOM_EMPTY)
				continue;

			auto search = roomDict.find(roomName);
			if (search == roomDict.end())
			{
				// specified room was not found in room definitions
				Log::e(STR_ROOM_MISSING, this->locPath.c_str(), roomName.c_str());
				this->unloadContent();
				return false;
			}

			// the reason why we copy the Room here is because the room grid
			// can contain multiple copies of the same base room, but each is
			// a separate entity, i.e. its state can change independently
			if (!this->rooms.set({ x, y }, std::make_shared<Room>(*search->second)))
			{
				this->unloadContent();
				return false;
			}
		}
	}

	// TODO sanity checks:
	// at least one MAS terminal
	// all rooms are reachable (no "holes", or "corner connections" between rooms)
	// [grind maps only] at least one exit

	// room grid loaded, now check if start room exists
	std::shared_ptr<Room> room = this->rooms.get(this->playerRoomCoords);
	if (room == nullptr)
	{
		Log::e(STR_START_ROOM_INVALID, this->locPath.c_str(), this->playerRoomCoords.x, this->playerRoomCoords.y);
		this->unloadContent();
		return false;
	}
	this->currentRoom = room;

	Log::d(STR_LOADED_LOCATION_CONTENT, this->locPath.c_str());
	return true;
}

void Location::unloadContent()
{
	this->backgroundFullSprite.clearPtr();
	this->rooms.clear();
}

std::string Location::getId()
{
	return this->id;
}

std::string Location::getTitle()
{
	return this->title;
}

std::string Location::getDescription()
{
	return this->description;
}

sf::Vector2u Location::getWorldMapCoords()
{
	return this->worldMapCoords;
}

bool Location::getIsBasecamp()
{
	return this->isBasecamp;
}

bool Location::getIsWorldMapIconBig()
{
	return this->isWorldMapIconBig;
}

uint Location::getRecommendedLevel()
{
	return this->recommendedLevel;
}

std::string Location::getWorldMapIconId()
{
	return this->worldMapIconId;
}

bool Location::gotoRoom(Direction direction)
{
	sf::Vector2u newCoords = this->playerRoomCoords;

	if (direction == DIR_UP)
	{
		if (newCoords.y == 0)
			return false; // can't go any higher

		newCoords.y -= 1;
	}
	else if (direction == DIR_LEFT)
	{
		if (newCoords.x == 0)
			return false; // can't go any more left

		newCoords.x -= 1;
	}
	else if (direction == DIR_DOWN)
	{
		newCoords.y += 1;
	}
	else if (direction == DIR_RIGHT)
	{
		newCoords.x += 1;
	}

	std::shared_ptr<Room> room = this->rooms.get(newCoords);
	if (room == nullptr)
		// the system you're searching for doesn't exist.
		// impossible! perhaps the archives are incomplete?
		return false;

	this->playerRoomCoords = newCoords;
	currentRoom = room;
	return true;
}

sf::Vector2u Location::getPlayerRoomCoords()
{
	return this->playerRoomCoords;
}

void Location::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	if (this->currentRoom->getDrawBackgroundFull())
		target.draw(this->backgroundFullSprite.sprite, states); // note: can be empty

	// note: stupid const methods are stupid.
	// dereferencing currentRoom is not allowed, but calling ::draw() on it is allowed
	//target.draw(*this->currentRoom, states);
	this->currentRoom->draw(target, states);
}
