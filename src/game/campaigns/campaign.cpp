#include <filesystem>
#include "campaign.hpp"
#include "../util/util.hpp"
#include "../consts.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

bool Campaign::load(std::string campaignDir, ResourceManager& resMgr)
{
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

	return true;
}

std::string Campaign::getTitle()
{
	return this->title;
}

std::string Campaign::getDescription()
{
	return this->description;
}
