#include <unordered_map>
#include "../resource_manager.hpp"
#include "location.hpp"
#include "../hud/log.hpp"

class Campaign
{
	private:
		std::unordered_map<std::string, std::unique_ptr<Location>> locations; // maps location id to location. location id is just dir name

	public:
		bool load(std::string campaignDir, ResourceManager& resMgr, Log &log);
};
