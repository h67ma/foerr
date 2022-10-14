#include "campaign.hpp"
#include <filesystem>
#include "../util/json.hpp"
#include "../consts.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"


Campaign::Campaign(ResourceManager &resMgr) : resMgr(resMgr)
{
	// big iron on his hip
}

/**
 * Loads a campaign from path. If previous campaign is loaded, it will be automatically unloaded first.
 *
 * @param campaignDir path to campaign directory containing campaign index file.
 * @return true if load succeeded
 * @return false if load failed
 */
bool Campaign::load(std::string campaignDir)
{
	Log::d(STR_CAMPAIGN_LOADING, campaignDir.c_str());

	this->unload();

	// load basic campaign infos

	std::string indexPath = pathCombine(campaignDir, std::string(FILENAME_INDEX));
	json root;
	if (!loadJsonFromFile(root, indexPath))
	{
		Log::e(STR_CAMPAIGN_LOAD_ERR, indexPath.c_str());
		return false;
	}

	if (!parseJsonKey<std::string>(root, indexPath.c_str(), FOERR_JSON_KEY_TITLE, this->title))
		return false;

	if (!parseJsonKey<std::string>(root, indexPath.c_str(), FOERR_JSON_KEY_DESCRIPTION, this->description))
		return false;

	// TODO translate title & description

	if (!parseJsonKey<std::string>(root, indexPath.c_str(), FOERR_JSON_KEY_START_LOC, this->startLocation))
		return false;

	if (!parseJsonKey<std::string>(root, indexPath.c_str(), FOERR_JSON_KEY_WORLDMAP_BACKGROUND, this->worldMapBackgroundId))
		return false;

	// load locations inside this campaign

	std::string locationsDir = pathCombine(campaignDir, std::string(DIR_LOCATIONS));
	std::filesystem::directory_iterator iter;

	try
	{
		iter = std::filesystem::directory_iterator(locationsDir);
	}
	catch(const std::filesystem::filesystem_error &ex)
	{
		Log::e(STR_FILE_NOT_FOUND, ex.what());
		return false;
	}

	// TODO? don't load all locations at the same time. we could load locations
	// when they are entered (maybe leaving basecamps loaded at all times)
	// that's problematic though, as player could start playing, first location loads,
	// player plays a bit, then enters other location and there's load error...
	// what then? this is unacceptable. a somewhat compromise solution would be to only
	// load resources, but then we also can end up in situation where some file is e.g.
	// missing in the middle of playing.
	// the best solution would be to check if all files are present and don't have any errors,
	// and somehow lock changing those files until campaign is unloaded.
	// for now let's just load everything at start
	for (const std::filesystem::directory_entry &entry : iter)
	{
		std::string locId = entry.path().stem().string();
		std::string locPath = entry.path().string();

		this->locations.emplace_back(locId, locPath);
		// initially we only load metadata such as name, description, world map details, etc.
		// rooms are loaded separately via ::loadContent() when entering a location.
		if (!this->locations.back().loadMeta())
		{
			// unload everything
			// mission failed, we'll get em next time
			Log::e(STR_LOADING_LOCATION_META_ERROR, locId.c_str());
			this->unload();
			return false;
		}
	}

	// TODO load other stuffs (most probably campaign-specific resources, so just /audio, /texture, etc)
	// TODO restore game state from save

	// TODO restore from save
	if (!this->changeLocationById(this->startLocation))
		return false;

	this->loaded = true;
	Log::d(STR_CAMPAIGN_LOADED, campaignDir.c_str());
	return true;
}

void Campaign::unload()
{
	Log::d(STR_CAMPAIGN_UNLOADING);
	this->title = "";
	this->description = "";
	this->startLocation = "";
	this->currentLocationIdx = -1;
	this->locations.clear();
	this->loaded = false;
	this->resMgr.cleanUnused();
	Log::d(STR_CAMPAIGN_UNLOADED);
}

std::string Campaign::getTitle()
{
	return this->title;
}

std::string Campaign::getDescription()
{
	return this->description;
}

std::string Campaign::getWorldMapBackground()
{
	return this->worldMapBackgroundId;
}

std::vector<Location>& Campaign::getLocations()
{
	return this->locations;
}

uint Campaign::getCurrentLocationIdx()
{
	return this->currentLocationIdx;
}

/**
 * Wow, such a brilliant name for a method.
 * Basically we want to unload rooms of every location, but keep all already
 * loaded basecamps because there's a good chance the player will be returning
 * to these again, and we want the transition to be quick.
 * Also we don't unload the previous location if player moved from non-basecamp
 * to basecamp, because player could then return to playing after restocking.
 *
 * @param newIdx index of a location which we want to load now, so we won't unload it :)
 */
void Campaign::unloadSomeLocations(uint newIdx)
{
	uint keepIdx = -1;
	// when loading first location after creating this object,
	// or after reset, currentLocationIdx is -1
	if (this->currentLocationIdx != -1 &&
		!this->locations[this->currentLocationIdx].isBasecamp() &&
		this->locations[newIdx].isBasecamp())
		keepIdx = this->currentLocationIdx;

	// we can't just unload last location (on the established conditions,
	// see method description).
	// consider the following sequence: player is in a non-basecamp location,
	// travels to a basecamp, then travels to another non-basecamp. we would
	// then *skip* unloading the first location in this example. this is why
	// we need to unload all which are not needed, just to be sure.
	for (uint i = 0; i < this->locations.size(); i++)
	{
		if (i == newIdx || i == keepIdx || this->locations[i].isBasecamp())
			continue;

		this->locations[i].unloadContent();
	}

	this->resMgr.cleanUnused();
}

bool Campaign::changeLocationByIndex(uint newIdx)
{
	if (newIdx >= this->locations.size())
	{
		Log::e(STR_IDX_OUTTA_BOUNDS);
		return false;
	}

	// load the new location. don't unload the old one yet, as new
	// one might fail to load and then we need to keep the old one
	if (!this->locations[newIdx].loadContent(this->resMgr))
	{
		Log::e(STR_LOADING_LOCATION_CONTENT_ERROR, this->locations[newIdx].getId().c_str());
		return false;
	}

	this->unloadSomeLocations(newIdx);

	this->currentLocationIdx = newIdx;

	Log::d(STR_LOC_CHANGED, this->locations[newIdx].getId().c_str());
	return true;
}

bool Campaign::changeLocationById(std::string locId)
{
	for (auto it = this->locations.begin(); it != this->locations.end(); it++)
	{
		if (it->getId() == locId)
		{
			return this->changeLocationByIndex(static_cast<uint>(std::distance(this->locations.begin(), it)));
		}
	}

	Log::e(STR_LOC_NOT_FOUND, locId.c_str());
	return false;
}

bool Campaign::isLoaded()
{
	return this->loaded;
}

bool Campaign::gotoRoom(Direction direction)
{
	if (!this->loaded)
		return false;

	return this->locations[this->currentLocationIdx].gotoRoom(direction);
}

sf::Vector2u Campaign::getPlayerRoomCoords()
{
	if (!this->loaded)
		return { 0, 0 }; // well, what can you do

	return this->locations[this->currentLocationIdx].getPlayerRoomCoords();
}

void Campaign::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	// TODO? probably could be optimized by storing a ptr to current loc
	target.draw(this->locations[this->currentLocationIdx], states);
}
