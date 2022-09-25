#include <json/reader.h>
#include "location.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

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

	Json::Value roomsNode = root[FOERR_JSON_KEY_ROOMS];
	if (!roomsNode.isArray())
	{
		Log::e(STR_INVALID_TYPE, this->locPath.c_str(), FOERR_JSON_KEY_ROOMS);
		return false;
	}

	// initial pass, check if exactly one room is marked as start room, set start coords,
	// and also calculate the size of room grid based on rooms coords.
	bool foundStart = false;
	sf::Vector2u gridDimens(0, 0);
	sf::Vector2u startRoomCoords;
	for (uint i = 0; i < roomsNode.size(); i++)
	{
		sf::Vector2u roomCoords;
		if (!parseJsonVector2uKey(roomsNode[i], this->locPath.c_str(), FOERR_JSON_KEY_COORDS, roomCoords))
		{
			this->unloadContent();
			return false;
		}

		// adjust required grid size if needed
		if ((roomCoords.x + 1) > gridDimens.x)
			gridDimens.x = roomCoords.x + 1;
		if ((roomCoords.y + 1) > gridDimens.y)
			gridDimens.y = roomCoords.y + 1;

		bool thisStart = false;
		parseJsonBoolKey(roomsNode[i], this->locPath.c_str(), FOERR_JSON_KEY_IS_START, thisStart, true);

		if (thisStart)
		{
			if (foundStart)
			{
				Log::e(STR_DUPLICATE_START_ROOM, this->locPath.c_str(), roomCoords.x, roomCoords.y);
				this->unloadContent();
				return false;
			}

			startRoomCoords = roomCoords;
			foundStart = true;
		}
	}

	if (!foundStart)
	{
		Log::e(STR_MISSING_START_ROOM, this->locPath.c_str());
		this->unloadContent();
		return false;
	}

	// if there really are no rooms, we'll fail before reaching this
	// because start room won't be found. still better check.
	if (gridDimens.x == 0 || gridDimens.y == 0)
	{
		Log::e(STR_NO_ROOMS_DEFINED, this->locPath.c_str());
		this->unloadContent();
		return false;
	}

	this->rooms.setDimens(gridDimens);

	// second pass, now we have set the grid size and know which room is the start one
	for (uint i = 0; i < roomsNode.size(); i++)
	{
		// yes, we need to read coords again
		sf::Vector2u roomCoords;
		if (!parseJsonVector2uKey(roomsNode[i], this->locPath.c_str(), FOERR_JSON_KEY_COORDS, roomCoords))
		{
			this->unloadContent();
			return false;
		}

		if (this->rooms.get(roomCoords) != nullptr)
		{
			Log::e(STR_DUPLICATE_ROOM_IN_SAME_COORDS, this->locPath.c_str(), roomCoords.x, roomCoords.y);
			this->unloadContent();
			return false;
		}

		std::shared_ptr<Room> room = std::make_shared<Room>();
		if (!room->load(roomsNode[i], this->locPath.c_str()))
		{
			this->unloadContent();
			return false;
		}

		if (!this->rooms.set(roomCoords, room))
		{
			// can't imagine how this could possibly fail, but check just to be sure
			this->unloadContent();
			return false;
		}
	}

	this->currentRoom = this->rooms.moveTo(startRoomCoords);
	if (this->currentRoom == nullptr)
	{
		Log::e(STR_ROOM_MISSING_COORDS, this->locPath.c_str(), startRoomCoords.x, startRoomCoords.y);
		this->unloadContent();
		return false;
	}

	// TODO sanity checks:
	// at least one MAS terminal
	// all rooms are reachable (every room has connection to at least one other room, unless the location consists of only one room)
	// [grind maps only] at least one exit

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
	std::shared_ptr<Room> newRoom = this->rooms.moveToNear(direction);
	if (newRoom == nullptr)
		return false;

	this->currentRoom = newRoom;
	return true;
}

sf::Vector2u Location::getPlayerRoomCoords()
{
	return this->rooms.getCurrentCoords();
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
