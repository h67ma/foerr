#include <filesystem>
#include "campaign.hpp"
#include "../util/util.hpp"
#include "../consts.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

/**
 * Loads a campaign from path. If previous campaign is loaded, it will be automatically unloaded first.
 *
 * @param campaignDir path to campaign directory containing campaign index file.
 * @param resMgr reference to resource manager
 * @return true if load succeeded
 * @return false if load failed
 */
bool Campaign::load(std::string campaignDir, ResourceManager &resMgr)
{
	Log::d(STR_CAMPAIGN_LOADING, campaignDir.c_str());

	this->unload(resMgr);

	// load basic campaign infos

	std::string indexPath = pathCombine(campaignDir, std::string(FILENAME_INDEX));
	Json::Value root;
	if (!loadJsonFromFile(root, indexPath))
	{
		Log::e(STR_CAMPAIGN_LOAD_ERR, indexPath.c_str());
		return false;
	}

	if (!parseJsonStringKey(root, indexPath.c_str(), FOERR_JSON_KEY_TITLE, this->title))
		return false;

	if (!parseJsonStringKey(root, indexPath.c_str(), FOERR_JSON_KEY_DESCRIPTION, this->description))
		return false;

	// TODO translate title & description

	if (!parseJsonStringKey(root, indexPath.c_str(), FOERR_JSON_KEY_START_LOC, this->startLocation))
		return false;

	if (!parseJsonStringKey(root, indexPath.c_str(), FOERR_JSON_KEY_WORLDMAP_BACKGROUND, this->worldMapBackgroundId))
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

		this->locations.emplace_back(locId);
		if (!this->locations.back().load(locPath, resMgr))
		{
			// unload everything
			// mission failed, we'll get em next time
			Log::e(STR_LOADING_LOCATION_ERROR, locId.c_str());
			this->unload(resMgr);
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

void Campaign::unload(ResourceManager &resMgr)
{
	Log::d(STR_CAMPAIGN_UNLOADING);
	this->title = "";
	this->description = "";
	this->startLocation = "";
	this->currentLocationIdx = 0;
	this->locations.clear();
	this->loaded = false;
	resMgr.cleanUnused();
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

bool Campaign::changeLocation(uint locIdx)
{
	if (locIdx >= this->locations.size())
	{
		Log::e(STR_IDX_OUTTA_BOUNDS);
		return false;
	}

	this->currentLocationIdx = locIdx;
	Log::d(STR_LOC_CHANGED, this->locations[locIdx].getId().c_str());
	return true;
}

bool Campaign::changeLocationById(std::string locId)
{
	for (auto it = this->locations.begin(); it != this->locations.end(); it++)
	{
		if (it->getId() == locId)
		{
			this->currentLocationIdx = static_cast<uint>(std::distance(this->locations.begin(), it));
			Log::d(STR_LOC_CHANGED, locId.c_str());
			return true;
		}
	}

	Log::e(STR_LOC_NOT_FOUND, locId.c_str());
	return false;
}

bool Campaign::isLoaded()
{
	return this->loaded;
}

void Campaign::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->locations[this->currentLocationIdx], states);
}
