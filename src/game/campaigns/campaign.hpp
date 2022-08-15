#include <unordered_map>
#include "../resource_manager.hpp"
#include "location.hpp"

class Campaign
{
	private:
		std::string title;
		std::string description;
		std::unordered_map<std::string, std::unique_ptr<Location>> locations; // maps location id to location. location id is just dir name

	public:
		bool load(std::string campaignDir, ResourceManager& resMgr);
		std::string getTitle();
		std::string getDescription();
};
