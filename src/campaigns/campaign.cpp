// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "campaign.hpp"

#include <string>

#include "../consts.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"
#include "../util/json.hpp"

Campaign::Campaign(ResourceManager& resMgr) : resMgr(resMgr), player(resMgr)
{
	// big iron on his hip
}

/**
 * Loads a campaign from path. If previous campaign is loaded, it will be automatically unloaded first.
 * Also loads materials into Material Manager.
 *
 * Campaign file structure:
 * {
 *	"api_version": 1,
 *	"title": "Test",
 *	"description": "Test campaign.",
 *	"start_location": "test_location",
 *	"worldmap_background": "path/to/world_map.png"
 * }
 *
 * Locations file structure:
 * {
 *	"api_version": 1,
 *	"locations": {
 *		"location_id": {
 *			// see location.cpp
 *		},
 *		...
 *	}
 * }
 *
 * @param campaignDir path to campaign directory containing campaign index file.
 * @return true if load succeeded
 * @return false if load failed
 */
bool Campaign::load(const std::string& campaignId)
{
	Log::d(STR_CAMPAIGN_LOADING, campaignId.c_str());

	this->unload();

	// load basic campaign infos

	this->id = campaignId;

	const std::string campaignDir = pathCombine(PATH_CAMPAIGNS, campaignId);
	std::string indexPath = pathCombine(campaignDir, FILENAME_INDEX);
	nlohmann::json root;
	if (!loadJsonFromFile(root, indexPath))
	{
		Log::e(STR_CAMPAIGN_LOAD_ERR, indexPath.c_str());
		return false;
	}

	if (!parseJsonKey<std::string>(root, indexPath, FOERR_JSON_KEY_TITLE, this->title))
		return false;

	if (!parseJsonKey<std::string>(root, indexPath, FOERR_JSON_KEY_DESCRIPTION, this->description))
		return false;

	// TODO translate title & description

	std::string startLocation;
	if (!parseJsonKey<std::string>(root, indexPath, FOERR_JSON_KEY_START_LOC, startLocation))
		return false;

	// load metadata for all locations inside this campaign.
	// Initially we only load metadata such as name, description, world map details, etc. Rooms are loaded separately
	// via ::loadContent() when entering a location. There's a possibility that some location will fail to load in the
	// middle of gameplay, but even then the player could save game and fix/report the faulty location without losing
	// progress. Metadata for all locations is kept in a single file, separate from room data (and background big path),
	// to avoid processing all files containing all room data when we only want to get metadata for each location to
	// display it on the world map.
	// TODO? we could add a button in campaign selector which would trigger test-loading all locations along with all
	// rooms.

	std::string locMetaPath = pathCombine(campaignDir, PATH_LOCATIONS_META);
	root.clear();
	if (!loadJsonFromFile(root, locMetaPath))
	{
		Log::e(STR_CAMPAIGN_LOAD_ERR, locMetaPath.c_str());
		return false;
	}

	auto locsSearch = root.find(FOERR_JSON_KEY_LOCATIONS);
	if (locsSearch == root.end())
	{
		Log::e(STR_MISSING_KEY, locMetaPath.c_str(), FOERR_JSON_KEY_LOCATIONS);
		return false;
	}

	if (!locsSearch->is_object())
	{
		Log::e(STR_INVALID_TYPE, locMetaPath.c_str(), FOERR_JSON_KEY_LOCATIONS);
		return false;
	}

	for (const auto& locNode : locsSearch->items())
	{
		std::string locId = locNode.key();
		if (locId.length() == 0)
		{
			Log::e(STR_LOC_ID_ZEROLEN, locMetaPath.c_str());
			return false;
		}

		std::shared_ptr<Location> loc = std::make_shared<Location>(locId, this->player);
		if (!loc->loadMeta(locNode.value(), campaignDir))
		{
			// unload everything
			// mission failed, we'll get em next time
			Log::e(STR_LOADING_LOCATION_META_ERROR, locId.c_str());
			this->unload();
			return false;
		}

		this->locations.emplace(locId, loc);
	}

	// TODO load other stuffs (most probably campaign-specific resources, so just /audio, /texture, etc)
	// TODO restore game state from save

	// TODO restore from save

	if (!this->matMgr.load())
		return false;

	if (!this->objMgr.load())
		return false;

	if (!this->changeLocation(startLocation))
		return false;

	Log::d(STR_CAMPAIGN_LOADED, campaignDir.c_str());
	return true;
}

void Campaign::unload()
{
	Log::d(STR_CAMPAIGN_UNLOADING);
	this->title = "";
	this->description = "";
	this->currentLocation = nullptr;
	this->lastUnloadableLocation = nullptr;
	this->locations.clear();
	this->resMgr.cleanUnused();
	Log::d(STR_CAMPAIGN_UNLOADED);
}

std::string Campaign::getId() const
{
	return this->id;
}

std::string Campaign::getTitle() const
{
	return this->title;
}

std::string Campaign::getDescription() const
{
	return this->description;
}

Player& Campaign::getPlayer()
{
	return this->player;
}

const std::unordered_map<std::string, std::shared_ptr<Location>>& Campaign::getLocations() const
{
	return this->locations;
}

const std::shared_ptr<Location> Campaign::getCurrentLocation() const
{
	return this->currentLocation;
}

/**
 * Searches Locations by locId and returns a pointer to Location, or nullptr if Location with such id does not exist.
 *
 * @param locId requested Location id
 * @return const shared pointer to requested Location, or nullptr if not found
 */
const std::shared_ptr<Location> Campaign::getLocation(const std::string& locId) const
{
	const auto search = this->locations.find(locId);
	if (search == this->locations.end())
		return nullptr;

	return search->second;
}

/**
 * Changes current location to the one specified by newLocId and in some cases unloads the previous location.
 *
 * @param newLocId new location id
 * @return true if location change was successful
 * @return false if location change failed
 */
bool Campaign::changeLocation(const std::string& newLocId)
{
	if (this->currentLocation != nullptr && this->currentLocation->getId() == newLocId)
		return true; // we're already in this location, silly

	auto newLocSearch = this->locations.find(newLocId);
	if (newLocSearch == this->locations.end())
	{
		Log::e(STR_LOC_NOT_FOUND, newLocId.c_str());
		return false;
	}

	std::shared_ptr<Location> newLoc = newLocSearch->second;

	// load the new location. don't unload the old one yet, as the new one might fail to load and then we need to keep
	// the old one
	if (!newLoc->loadContent(this->resMgr, this->matMgr, this->objMgr))
	{
		Log::e(STR_LOADING_LOCATION_CONTENT_ERROR, newLocSearch->first.c_str());
		return false;
	}

	// it's important to unload old location only after loading the new one, because there's a good chance that the new
	// location uses some of the resources which are already loaded by the old one. therefore we avoid unloading and
	// immediately loading the same resource.

	// sometimes we don't actually unload the old location. the unloading should happen only if the player was
	// previously visiting a non-basecamp location, and currently we're loading another non-basecamp location.
	// we never unload already loaded basecamps, because there's a good chance the player will be returning to these,
	// and we want the transition to be as quick as possible.
	// vice versa, we also don't unload the previous location if player moved from non-basecamp to basecamp, because
	// the player could then return to playing after e.g. restocking.
	// however, under established conditions, we can't simply unload last location.
	// consider the following sequence: player is in a non-basecamp location, travels to a basecamp, then travels to
	// another non-basecamp. we would then *skip* unloading the first location in this example. this is why we need to
	// keep track of last "unloadable" location.

	// no point unloading anything if current location is not set (this means that the Campaign was just created or has
	// been unloaded)
	if (this->currentLocation != nullptr)
	{
		if (!this->currentLocation->isBasecamp() && newLoc->isBasecamp())
		{
			// non-basecamp -> basecamp: set last unloadable as old location, as it's a non-basecamp
			this->lastUnloadableLocation = this->currentLocation;
		}
		else if (!this->currentLocation->isBasecamp() && !newLoc->isBasecamp())
		{
			// non-basecamp -> non-basecamp: unload old location
			this->currentLocation->unloadContent();
			this->resMgr.cleanUnused();
			// we don't care about last unloadable loc in this case.
			// we'll take care of it when we transition to a basecamp (see first branch)
		}
		else if (this->currentLocation->isBasecamp() && !newLoc->isBasecamp())
		{
			// basecamp -> non-basecamp: unload last unloadable (if it exists and is not the location which we're
			// trying to load now) and unset it
			if (this->lastUnloadableLocation != nullptr && this->lastUnloadableLocation != newLoc)
			{
				this->lastUnloadableLocation->unloadContent();
				this->resMgr.cleanUnused();
			}

			this->lastUnloadableLocation = nullptr;
		}
		// else: basecamp -> basecamp: we don't unload anything
	}

	this->currentLocation = newLoc;

	sf::Vector2u spawnCoordsPx = this->currentLocation->getSpawnCoords() * CELL_SIDE_LEN;
	this->player.setPosition(spawnCoordsPx.x, spawnCoordsPx.y);

	Log::d(STR_LOC_CHANGED, this->currentLocation->getId().c_str());
	return true;
}

bool Campaign::isLoaded()
{
	return this->currentLocation != nullptr;
}

/**
 * Should only be used for debug purposes.
 * Enters a nearby Room if it's present. Player is moved to a default position (Room center).
 * Note: we can't use spawn coords, as we don't know the spawn coords for new Room just yet.
 */
bool Campaign::gotoRoom(Direction direction)
{
	if (this->currentLocation == nullptr)
		return false;

	return this->currentLocation->gotoRoom(direction, { ROOM_WIDTH_WITH_BORDER * CELL_SIDE_LEN / 2,
														ROOM_HEIGHT_WITH_BORDER * CELL_SIDE_LEN / 2 });
}

bool Campaign::gotoRoom(HashableVector3i coords)
{
	if (this->currentLocation == nullptr)
		return false;

	return this->currentLocation->gotoRoom(coords);
}

void Campaign::redraw()
{
	if (this->currentLocation == nullptr)
		return;

	this->currentLocation->redraw();
}

/**
 * Logs a message consisting of current Location name, Room coordinates, Player position, and Cell coordinates at
 * Player's position.
 *
 * If the Campaign is not loaded, nothing will happen.
 *
 * This should be used *only* for debug purposes.
 */
void Campaign::logWhereAmI()
{
	if (this->currentLocation == nullptr)
		return;

	sf::Vector3i coords = this->currentLocation->getPlayerRoomCoords();
	sf::Vector2f playerCoords = this->player.getPosition();

	Log::i(STR_DEBUG_WHEREAMI, this->currentLocation->getId().c_str(), coords.x, coords.y, coords.z, playerCoords.x,
		   playerCoords.y, static_cast<int>(playerCoords.x / CELL_SIDE_LEN),
		   static_cast<int>(playerCoords.y / CELL_SIDE_LEN));
}

void Campaign::tick(uint lastFrameDurationUs)
{
	if (this->currentLocation == nullptr)
		return;

	this->currentLocation->tick(lastFrameDurationUs);
}

void Campaign::nextFrame()
{
	// we could move animating the Player to Location, i.e. here call this->currentLocation->nextFrame(). then inside
	// Location::nextFrame() we could check if Room transition is in progress, and if yes, not animate the player.
	// however that few extra steps each frame might potentially have a negative effect on performance, and animating
	// the player during Room transition is not super expensive, so let's keep it as is for now.
	this->player.nextFrame();
}

void Campaign::teleportPlayer(sf::Vector2f position)
{
	if (position.x < 0 || position.x > GAME_AREA_WIDTH || position.y < 0 || position.y > GAME_AREA_HEIGHT)
		return;

	this->player.setPosition(position);
}

/**
 * Re-setups sprites in current location's current room, which causes new texture variants to be picked.
 */
void Campaign::rerollObjVariants()
{
	if (this->currentLocation == nullptr)
		return;

	this->currentLocation->rerollObjVariants(this->resMgr, this->objMgr);
}

/**
 * Sets lights state for current room in current location.
 * Re-setups sprites with new variants so that the change is applied.
 */
void Campaign::setRoomLightsState(enum LightObjectsState state)
{
	if (this->currentLocation == nullptr)
		return;

	this->currentLocation->setRoomLightsState(state);
	this->rerollObjVariants();
}

void Campaign::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(*this->currentLocation, states);
}
