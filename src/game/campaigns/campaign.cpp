#include <filesystem>
#include "campaign.hpp"
#include "../util/util.hpp"
#include "../consts.hpp"
#include "../util/i18n.hpp"

bool Campaign::load(std::string campaignDir, ResourceManager& resMgr, Log &log)
{
	std::string locationsDir = pathCombine(campaignDir, std::string(DIR_LOCATIONS));
	std::filesystem::directory_iterator iter;

	try
	{
		iter = std::filesystem::directory_iterator(locationsDir);
	}
	catch(const std::filesystem::filesystem_error &ex)
	{
		log.log(LOG_ERROR, STR_FILE_NOT_FOUND, ex.what());
		return false;
	}

	for (const std::filesystem::directory_entry& entry : iter)
	{
		std::string locId = entry.path().filename().string();
		std::string locPath = entry.path().string();
		Location *loc = new Location();
		if (!loc->load(locPath, resMgr, log))
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
