#pragma once

#include <unordered_map>
#include "../resource_manager.hpp"
#include "location.hpp"

class Campaign : public sf::Drawable
{
	private:
		bool loaded = false;
		std::string title;
		std::string description;
		std::string startLocation;
		std::string worldMapBackgroundId;
		Location *currentLocation = nullptr;
		std::unordered_map<std::string, std::unique_ptr<Location>> locations; // maps location id to location. location id is just dir name

	public:
		bool load(std::string campaignDir, ResourceManager &resMgr);
		void unload(ResourceManager &resMgr);
		std::string getTitle();
		std::string getDescription();
		std::string getWorldMapBackground();
		const std::unordered_map<std::string, std::unique_ptr<Location>>& getLocations();
		bool changeLocation(std::string locKey);
		bool isLoaded();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
