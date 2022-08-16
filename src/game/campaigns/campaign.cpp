#include <filesystem>
#include "campaign.hpp"
#include "../util/util.hpp"
#include "../consts.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

bool Campaign::load(std::string campaignDir, ResourceManager& resMgr)
{
	this->locations.clear();

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
	for (const std::filesystem::directory_entry& entry : iter)
	{
		std::string locId = entry.path().filename().string();
		std::string locPath = entry.path().string();
		Location *loc = new Location();
		if (!loc->load(locPath, resMgr))
		{
			// unload everything
			// mission failed, we'll get em next time
			delete loc;
			this->locations.clear();
			return false;
		}
		this->locations[locId] = std::unique_ptr<Location>(loc);
	}

	// TODO load other stuffs (most probably campaign-specific resources, so just /audio, /texture, etc)
	// TODO restore game state from save

	// TODO restore from save
	if (!this->changeLocation(this->startLocation))
		return false;

	this->loaded = true;
	return true;
}

void Campaign::unload()
{
	this->title = "";
	this->description = "";
	this->startLocation = "";
	this->currentLocation = nullptr;
	this->locations.clear();
	this->loaded = false;
}

std::string Campaign::getTitle()
{
	return this->title;
}

std::string Campaign::getDescription()
{
	return this->description;
}

bool Campaign::changeLocation(std::string locKey)
{
	auto search = this->locations.find(locKey);
	if (search == this->locations.end())
	{
		Log::e(STR_LOC_NOT_FOUND, locKey.c_str());
		return false;
	}

	Location *loc = search->second.get();
	if (loc == nullptr)
	{
		Log::e(STR_LOC_EMPTY, locKey.c_str());
		return false;
	}

	this->currentLocation = loc;
	Log::d(STR_LOC_CHANGED, locKey.c_str());
	return true;
}

bool Campaign::isLoaded()
{
	return this->loaded;
}

void Campaign::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(*this->currentLocation, states);
}