#include "location.hpp"
#include <string>
#include <memory>
#include <SFML/Graphics/RenderTexture.hpp>
#include "../util/i18n.hpp"
#include "../hud/log.hpp"
#include "../util/json.hpp"

#define LOC_WORLDMAP_MAX 600 // max x/y coordinate of worldmap icons

Location::Location(std::string id, uint transitionTimeMs) : id(id), transitionTimeMs(transitionTimeMs)
{
	// "It's ghouls, I tell ya. Religious ghouls in rockets looking for a land to call their own."
}

/**
 * Loads location details, mainly for display on the world map.
 * Rooms and background big are loaded separately via ::loadContent().
 *
 * Location metadata node structure:
 * {
 *	"title": "Location Name",
 *	"description": "Description shown on world map page.",
 *	"grind": false,
 *	"basecamp": false,
 *	"rec_lvl": 42,					// optional
 *	"rooms": "rooms_file_id",		// optional, name of the rooms file, located in campaigns/campaign_id/rooms,
 *									   without extension. defaults to location id
 *	"worldmap_icon": "res/campaigns/test/hud/icons/surface.png",
 *	"worldmap_icon_big": false,		// optional, defaults to false
 *	"worldmap_coords": [123, 456],
 *	"rooms": [
 *		...	// see room.cpp
 *	]
 * }
 *
 * @param locMetaNode reference to json node containing only this location's metadata
 * @returns true if load succeeded
 * @returns false if load failed
 */
bool Location::loadMeta(const json &locMetaNode, const std::string &campaignDir)
{
	Log::v(STR_LOADING_LOCATION_META, this->id.c_str());

	if (!parseJsonKey<std::string>(locMetaNode, this->id, FOERR_JSON_KEY_TITLE, this->title))
		return false;

	if (!parseJsonKey<std::string>(locMetaNode, this->id, FOERR_JSON_KEY_DESCRIPTION, this->description))
		return false;

	// TODO translate title & description

	if (!parseJsonKey<bool>(locMetaNode, this->id, FOERR_JSON_KEY_TYPE_GRIND, this->grind))
		return false;

	if (!parseJsonKey<bool>(locMetaNode, this->id, FOERR_JSON_KEY_TYPE_BASECAMP, this->basecamp))
		return false;

	if (this->basecamp && this->grind)
	{
		// something stinks here...
		Log::e(STR_LOC_INVALID_TYPES, this->id.c_str());
		return false;
	}

	parseJsonKey<uint>(locMetaNode, this->id, FOERR_JSON_KEY_RECOMMENDED_LVL, this->recommendedLevel, true);

	std::string roomsFilename = "";
	parseJsonKey<std::string>(locMetaNode, this->id, FOERR_JSON_KEY_ROOMS, roomsFilename, true);

	// default to location id if not specified
	if (roomsFilename == "")
		roomsFilename = this->id;

	this->roomDataPath = pathCombine(campaignDir, std::string(PATH_DIR_ROOMS), roomsFilename + ".json");

	if (!parseJsonKey<std::string>(locMetaNode, this->id, FOERR_JSON_KEY_WORLDMAP_ICON, this->worldMapIconId))
		return false;

	if (!parseJsonVector2Key<uint>(locMetaNode, this->id, FOERR_JSON_KEY_WORLDMAP_COORDS, this->worldMapCoords))
		return false;

	if (this->worldMapCoords.x > LOC_WORLDMAP_MAX || this->worldMapCoords.y > LOC_WORLDMAP_MAX)
	{
		Log::e(STR_SUS_LARGE_VALUE, FOERR_JSON_KEY_WORLDMAP_COORDS);
		return false;
	}

	// not present -> default value (false)
	parseJsonKey<bool>(locMetaNode, this->id, FOERR_JSON_KEY_WORLDMAP_ICON_BIG, this->worldMapIconBig, true);

	Log::v(STR_LOADED_LOCATION_META, this->id.c_str());
	return true;
}

/**
 * Loads room data and big background.
 *
 * In case when loading fails, all previously allocated rooms will be automatically deallocated.
 *
 * Rooms file structure:
 * {
 *	"api_version": 1,
 *	"background_full": "path/to/img.png",	// optional
 *	"rooms": [
 *		{
 *			// see room.cpp
 *		},
 *		...
 *	]
 * }
 *
 * @param resMgr reference to resource manager object
 * @returns true if load succeeded
 * @returns false if load failed
 */
bool Location::loadContent(ResourceManager &resMgr, const MaterialManager &matMgr)
{
	std::string backgroundFullPath;
	json root;

	this->unloadContent();

	Log::v(STR_LOADING_LOCATION_CONTENT, this->id.c_str());

	if (!loadJsonFromFile(root, this->roomDataPath))
		return false;

	// not present -> black background
	if (parseJsonKey<std::string>(root, this->roomDataPath, FOERR_JSON_KEY_BACKGROUND_FULL, backgroundFullPath, true))
	{
		backgroundFullPath = pathCombine(PATH_BACKGROUNDS_FULL, backgroundFullPath + ".png");
		this->backgroundFullSprite.setTexture(resMgr.getTexture(backgroundFullPath));
	}

	auto roomsSearch = root.find(FOERR_JSON_KEY_ROOMS);
	if (roomsSearch == root.end())
	{
		Log::e(STR_MISSING_KEY, this->roomDataPath.c_str(), FOERR_JSON_KEY_ROOMS);
		return false;
	}

	if (!roomsSearch->is_array())
	{
		Log::e(STR_INVALID_TYPE, this->roomDataPath.c_str(), FOERR_JSON_KEY_ROOMS);
		return false;
	}

	// TODO this is a prime candidate for parallelization - run N threads, each gets a few rooms
	bool foundStart = false;
	HashableVector3i startRoomCoords;
	for (const auto &roomNode : (*roomsSearch))
	{
		HashableVector3i roomCoords;
		if (!parseJsonVector3iKey(roomNode, this->roomDataPath, FOERR_JSON_KEY_COORDS, roomCoords))
		{
			this->unloadContent();
			return false;
		}

		if (this->rooms.get(roomCoords) != nullptr)
		{
			Log::e(STR_DUPLICATE_ROOM_IN_SAME_COORDS, this->roomDataPath.c_str(),
				   roomCoords.x, roomCoords.y, roomCoords.z);
			this->unloadContent();
			return false;
		}

		bool thisStart = false;
		parseJsonKey<bool>(roomNode, this->roomDataPath, FOERR_JSON_KEY_IS_START, thisStart, true);

		// check if exactly one room is marked as start room
		if (thisStart)
		{
			if (foundStart)
			{
				Log::e(STR_DUPLICATE_START_ROOM, this->roomDataPath.c_str(), roomCoords.x, roomCoords.y, roomCoords.z);
				this->unloadContent();
				return false;
			}

			startRoomCoords = roomCoords;
			foundStart = true;
		}

		std::shared_ptr<Room> room = std::make_shared<Room>();

		// TODO currently we keep all Rooms of the Location loaded in memory, which seems fine, because we want to do
		// all the complicated setup when loading the Location, not when moving between Rooms. but this approach
		// consumes a lot of memory, keeping track of all Rooms, while we really only need one. a solution might be to
		// only keep the current and nearby Rooms loaded. nearby Rooms could be loaded (along with the resources they
		// need) in a background thread. we'd only need to keep some light-weight version of Location data, e.g. a json
		// object, or some kind of meta-location, with some of the data preliminarily parsed.
		if (!room->load(resMgr, matMgr, roomNode, this->roomDataPath))
		{
			this->unloadContent();
			return false;
		}

		this->rooms.set(roomCoords, room);
	}

	if (!foundStart)
	{
		// also covers the case where we have zero rooms
		Log::e(STR_MISSING_START_ROOM, this->roomDataPath.c_str());
		this->unloadContent();
		return false;
	}

	// enter the starting room
	this->currentRoom = this->rooms.moveTo(startRoomCoords);
	if (this->currentRoom == nullptr)
	{
		Log::e(STR_ROOM_MISSING_COORDS, this->roomDataPath.c_str(),
			   startRoomCoords.x, startRoomCoords.y, startRoomCoords.z);
		this->unloadContent();
		return false;
	}

	this->currentRoom->init();

	// TODO sanity checks:
	// - at least one MAS terminal
	// - [grind maps only] at least one exit
	// - all doors/vents/etc must have a counterpart in another room, so that all passages actually lead to other rooms

	// we could also check if room grid is valid, i.e. are all rooms reachable.
	// we'd have to implement some kind of DFS. checking if every room has at least one neighbor is not enough - two
	// rooms could be connected only to each other and not to the rest of the location, which would fool this check.
	// this would be nice to have, but could potentially limit the ability to create some interesting locations (e.g.
	// ones using teleportation to travel between different unconnected sections, or ones which can be entered from more
	// than one side). so for now let's assume that the creator of location will make sure that all required rooms are
	// reachable.

	Log::v(STR_LOADED_LOCATION_CONTENT, this->roomDataPath.c_str());
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

bool Location::isBasecamp()
{
	return this->basecamp;
}

bool Location::isWorldMapIconBig()
{
	return this->worldMapIconBig;
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

	if (this->transitionTimeMs == 0 || direction == DIR_BACK || direction == DIR_FRONT)
	{
		// no transition when transition duration is 0, or when changing the Z coordinate

		// old room no longer needed
		this->currentRoom->deinit();

		this->currentRoom = newRoom;
		this->currentRoom->init();

		this->roomTransitionInProgress = false;

		return true;
	}

	this->roomTransitionDirection = direction;

	sf::RenderTexture tmpTxt;

	// TODO? not sure why, but if we create (W * 2, H), then on *next call* (W, H * 2), the size will change, but it
	// will work as if the actual size was *still* the one we set on first call. maybe this has something to do with
	// NonCopyable. for now let's just use 2x as much memory for nothing :|
	tmpTxt.create(GAME_AREA_WIDTH * 2, GAME_AREA_HEIGHT * 2);

	tmpTxt.clear(sf::Color::Transparent);

	// render old room to transition texture

	// in case of right and down, the old room stays at (0, 0), and the new one is rendered at the side or below.
	// in case of left and up, the roles are reversed.
	// we assume that room's position is (0, 0), as we reset it at the end of room transition.

	if (direction == DIR_LEFT)
		this->currentRoom->setPosition(GAME_AREA_WIDTH, 0);
	else if (direction == DIR_UP)
		this->currentRoom->setPosition(0, GAME_AREA_HEIGHT);

	tmpTxt.draw(*this->currentRoom);

	// old room no longer needed
	this->currentRoom->deinit();

	// reset old room position
	this->currentRoom->setPosition(0, 0);

	this->currentRoom = newRoom;
	this->currentRoom->init();

	// render new room to the transition texture (same as old room, but in different position

	// reversed (see above)
	if (direction == DIR_RIGHT)
		this->currentRoom->setPosition(GAME_AREA_WIDTH, 0);
	else if (direction == DIR_DOWN)
		this->currentRoom->setPosition(0, GAME_AREA_HEIGHT);

	tmpTxt.draw(*this->currentRoom);

	// reset current room position
	this->currentRoom->setPosition(0, 0);

	tmpTxt.display();
	this->roomTransitionTxt = tmpTxt.getTexture();
	this->roomTransitionSprite.setTexture(this->roomTransitionTxt);

	// set initial transition sprite position
	if (direction == DIR_RIGHT || direction == DIR_DOWN)
		this->roomTransitionSprite.setPosition(0, 0);
	else if (direction == DIR_LEFT)
		this->roomTransitionSprite.setPosition(-GAME_AREA_WIDTH, 0);
	else if (direction == DIR_UP)
		this->roomTransitionSprite.setPosition(0, -GAME_AREA_HEIGHT);

	this->roomTransitionTimer.restart();
	this->roomTransitionInProgress = true;

	return true;
}

sf::Vector3i Location::getPlayerRoomCoords()
{
	// note: here we downcast HashableVector to Vector, as the specialized type is not needed anymore
	return this->rooms.getCurrentCoords();
}

void Location::updateState()
{
	if (this->roomTransitionInProgress)
	{
		uint elapsed = this->roomTransitionTimer.getElapsedTime().asMilliseconds();

		if (elapsed >= this->transitionTimeMs)
		{
			this->roomTransitionInProgress = false;
			return;
		}

		sf::Vector2f pos;

		switch (this->roomTransitionDirection)
		{
			case DIR_UP:
				pos.y = static_cast<int>(elapsed * GAME_AREA_HEIGHT / this->transitionTimeMs) - GAME_AREA_HEIGHT;
				break;
			case DIR_DOWN:
				pos.y = - static_cast<int>(elapsed * GAME_AREA_HEIGHT / this->transitionTimeMs);
				break;
			case DIR_LEFT:
				pos.x = static_cast<int>(elapsed * GAME_AREA_WIDTH / this->transitionTimeMs) - GAME_AREA_WIDTH;
				break;
			case DIR_RIGHT:
				pos.x = - static_cast<int>(elapsed * GAME_AREA_WIDTH / this->transitionTimeMs);
				break;
			default:
				// this should never happen
				this->roomTransitionInProgress = false;
				return;
		}

		this->roomTransitionSprite.setPosition(pos);
	}
}

void Location::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	// background full is drawn the same during transition and regular gameplay - it's "far away" so it shouldn't move
	target.draw(this->backgroundFullSprite, states); // note: can be empty

	if (!this->roomTransitionInProgress)
	{
		target.draw(*this->currentRoom, states);
	}
	else
	{
		target.draw(this->roomTransitionSprite, states);
	}
}
