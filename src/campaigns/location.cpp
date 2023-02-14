#include "location.hpp"

#include <string>
#include <memory>

#include <SFML/Graphics/RenderTexture.hpp>

#include "../settings/settings_manager.hpp"
#include "../util/json.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

#define LOC_WORLDMAP_MAX 600 // max x/y coordinate of worldmap icons

#define PLAYER_NEW_ROOM_OFFSET_H 60
#define PLAYER_NEW_ROOM_OFFSET_V_TOP 40
#define PLAYER_NEW_ROOM_OFFSET_V_BOTTOM 80 // platform jump

Location::Location(const std::string &id, Player &player) : id(id), player(player)
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

	// note: location can be a grind map and a basecamp at the same time

	parseJsonKey<uint>(locMetaNode, this->id, FOERR_JSON_KEY_RECOMMENDED_LVL, this->recommendedLevel, true);

	std::string roomsFilename;
	parseJsonKey<std::string>(locMetaNode, this->id, FOERR_JSON_KEY_ROOMS, roomsFilename, true);

	// default to location id if not specified
	if (roomsFilename.empty())
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
 * @param resMgr reference to Resource Manager object
 * @param matMgr reference to Material Manager object
 * @param objMgr reference to Object Manager object
 * @returns true if load succeeded
 * @returns false if load failed
 */
bool Location::loadContent(ResourceManager &resMgr, const MaterialManager &matMgr, const ObjectManager &objMgr)
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

		std::shared_ptr<Room> room = std::make_shared<Room>(this->player);

		// TODO currently we keep all Rooms of the Location loaded in memory, which seems fine, because we want to do
		// all the complicated setup when loading the Location, not when moving between Rooms. but this approach
		// consumes a lot of memory, keeping track of all Rooms, while we really only need one. a solution might be to
		// only keep the current and nearby Rooms loaded. nearby Rooms could be loaded (along with the resources they
		// need) in a background thread. we'd only need to keep some light-weight version of Location data, e.g. a json
		// object, or some kind of meta-location, with some of the data preliminarily parsed.
		if (!room->load(resMgr, matMgr, objMgr, roomNode, this->roomDataPath))
		{
			this->unloadContent();
			return false;
		}

		// note: we only validate geometry for unique (non-grind) locations
		if (!this->grind && !this->validateRoomGeometry(room, roomCoords))
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

/**
 * Validates geometry of a single Room, by checking if sides of all adjacent Rooms have the same layout of collider
 * Cells (based on presence of solid in a Cell). If there's no room on a given side, the validation also passes.
 *
 * This should be called in the same loop where Rooms are being loaded. This way all connections will be checked exactly
 * once (a given Room will check its given side only when the adjacent Room is present).
 *
 * Connections in the Z axis are not checked, as walls don't matter in that case.
 *
 * @return true if validation passed, false otherwise
 */
bool Location::validateRoomGeometry(const std::shared_ptr<Room> room, HashableVector3i roomCoords) const
{
	roomCoords.x -= 1;
	std::shared_ptr<Room> roomLeft = this->rooms.get(roomCoords);
	roomCoords.x += 2;
	std::shared_ptr<Room> roomRight = this->rooms.get(roomCoords);
	roomCoords.x -= 1;

	roomCoords.y -= 1;
	std::shared_ptr<Room> roomUp = this->rooms.get(roomCoords);
	roomCoords.y += 2;
	std::shared_ptr<Room> roomDown = this->rooms.get(roomCoords);
	roomCoords.y -= 1;

	if (roomLeft != nullptr)
	{
		for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
		{
			if (room->isCellCollider(0, y) != roomLeft->isCellCollider(ROOM_WIDTH_WITH_BORDER - 1, y))
			{
				Log::e(STR_ROOM_GEOMETRY_VAL_FAIL, roomCoords.x, roomCoords.y, roomCoords.z, 0, y);
				return false;
			}
		}
	}

	if (roomRight != nullptr)
	{
		for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
		{
			if (room->isCellCollider(ROOM_WIDTH_WITH_BORDER - 1, y) != roomRight->isCellCollider(0, y))
			{
				Log::e(STR_ROOM_GEOMETRY_VAL_FAIL, roomCoords.x, roomCoords.y, roomCoords.z,
					   ROOM_WIDTH_WITH_BORDER - 1, y);
				return false;
			}
		}
	}

	if (roomUp != nullptr)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			if (room->isCellCollider(x, 0) != roomUp->isCellCollider(x, ROOM_HEIGHT_WITH_BORDER - 1))
			{
				Log::e(STR_ROOM_GEOMETRY_VAL_FAIL, roomCoords.x, roomCoords.y, roomCoords.z, x, 0);
				return false;
			}
		}
	}

	if (roomDown != nullptr)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			if (room->isCellCollider(x, ROOM_HEIGHT_WITH_BORDER - 1) != roomDown->isCellCollider(x, 0))
			{
				Log::e(STR_ROOM_GEOMETRY_VAL_FAIL, roomCoords.x, roomCoords.y, roomCoords.z,
					   x, ROOM_HEIGHT_WITH_BORDER - 1);
				return false;
			}
		}
	}

	return true;
}

void Location::unloadContent()
{
	this->backgroundFullSprite.clearPtr();
	this->rooms.clear();
}

std::string Location::getId() const
{
	return this->id;
}

std::string Location::getTitle() const
{
	return this->title;
}

std::string Location::getDescription() const
{
	return this->description;
}

sf::Vector2u Location::getWorldMapCoords() const
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

uint Location::getRecommendedLevel() const
{
	return this->recommendedLevel;
}

std::string Location::getWorldMapIconId() const
{
	return this->worldMapIconId;
}

/**
 * Changes the current Room to a nearest Room in the specified direction.
 * If such Room does not exist, nothing will happen.
 * Moves the Player to a new position, specified by newPlayerCoords. Moving the Player needs to happen in this function
 * in order to display them correctly during Room transition.
 *
 * Also prepares the texture and timer used for Room transition animation.
 *
 * @param direction direction from current Room to go
 * @return true if Room was changed
 * @return false if current Room didn't change
 */
bool Location::gotoRoom(Direction direction, sf::Vector2f newPlayerCoords)
{
	std::shared_ptr<Room> newRoom = this->rooms.moveToNear(direction);
	if (newRoom == nullptr)
		return false;

	if (SettingsManager::roomTransitionDurationMs == 0 || direction == DIR_BACK || direction == DIR_FRONT)
	{
		// no transition when transition duration is 0, or when changing the Z coordinate

		// old room no longer needed
		this->currentRoom->deinit();

		this->player.setPosition(newPlayerCoords);

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

	// this trick is to ensure that the Player is not displayed twice during Room transition - once in old Room, once in
	// the new Room. same effect could also be achieved by setting some field in Room, which tells it to skip drawing
	// the Player, but simply moving the Player outside visible area does the job just fine.
	this->player.setPosition(-500, -500);

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

	this->player.setPosition(newPlayerCoords);

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

bool Location::gotoRoom(HashableVector3i coords)
{
	std::shared_ptr<Room> newRoom = this->rooms.moveTo(coords);
	if (newRoom == nullptr)
		return false;

	this->currentRoom->deinit();
	this->currentRoom = newRoom;
	this->currentRoom->init();

	this->player.setPosition(static_cast<sf::Vector2f>(this->currentRoom->getSpawnCoords() * CELL_SIDE_LEN));

	return true;
}

void Location::redraw()
{
	this->currentRoom->init();
}

sf::Vector2u Location::getSpawnCoords() const
{
	return this->currentRoom->getSpawnCoords();
}

sf::Vector3i Location::getPlayerRoomCoords() const
{
	// note: here we downcast HashableVector to Vector, as the specialized type is not needed anymore
	return this->rooms.getCurrentCoords();
}

void Location::tick(uint lastFrameDurationUs)
{
	if (!this->roomTransitionInProgress)
	{
		this->player.tick(lastFrameDurationUs);

		// check if the player has walked into screen edge.
		// if nearby Room exists, move to it.
		// if nearby Room is not present, stop the player.
		float currentY = this->player.getPosition().y;

		if (this->player.getPosition().x < PLAYER_W2)
		{
			if (!this->gotoRoom(DIR_LEFT, { GAME_AREA_WIDTH - PLAYER_NEW_ROOM_OFFSET_H, currentY }))
			{
				this->player.setPosition(PLAYER_W2, currentY);
				this->player.stopHorizontal();
			}
		}
		else if (this->player.getPosition().x > GAME_AREA_WIDTH - PLAYER_W2)
		{
			if (!this->gotoRoom(DIR_RIGHT, { PLAYER_NEW_ROOM_OFFSET_H, currentY }))
			{
				this->player.setPosition(GAME_AREA_WIDTH - PLAYER_W2, currentY);
				this->player.stopHorizontal();
			}
		}

		// need to read position again, in case the Player touched Room corner and their horizontal position has already
		// been changed (see above)
		float currentX = this->player.getPosition().x;

		if (this->player.getPosition().y < PLAYER_H2)
		{
			if (!this->gotoRoom(DIR_UP, { currentX, GAME_AREA_HEIGHT - PLAYER_NEW_ROOM_OFFSET_V_BOTTOM }))
			{
				this->player.setPosition(currentX, PLAYER_H2);
				this->player.stopVertical();
			}
		}
		else if (this->player.getPosition().y > GAME_AREA_HEIGHT - PLAYER_H2)
		{
			if (!this->gotoRoom(DIR_DOWN, { currentX, PLAYER_NEW_ROOM_OFFSET_V_TOP }))
			{
				this->player.setPosition(currentX, GAME_AREA_HEIGHT - PLAYER_H2);
				this->player.stopVertical();
			}
		}
	}
	else
	{
		uint elapsed = this->roomTransitionTimer.getElapsedTime().asMilliseconds();

		if (elapsed >= SettingsManager::roomTransitionDurationMs)
			this->roomTransitionInProgress = false;

		sf::Vector2f pos;

		switch (this->roomTransitionDirection)
		{
			case DIR_UP:
				pos.y = static_cast<int>(elapsed * GAME_AREA_HEIGHT / SettingsManager::roomTransitionDurationMs) - GAME_AREA_HEIGHT;
				break;
			case DIR_DOWN:
				pos.y = - static_cast<int>(elapsed * GAME_AREA_HEIGHT / SettingsManager::roomTransitionDurationMs);
				break;
			case DIR_LEFT:
				pos.x = static_cast<int>(elapsed * GAME_AREA_WIDTH / SettingsManager::roomTransitionDurationMs) - GAME_AREA_WIDTH;
				break;
			case DIR_RIGHT:
				pos.x = - static_cast<int>(elapsed * GAME_AREA_WIDTH / SettingsManager::roomTransitionDurationMs);
				break;
			default:
				// this should never happen
				this->roomTransitionInProgress = false;
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
