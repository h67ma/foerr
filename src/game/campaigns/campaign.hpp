#pragma once

#include <vector>
#include "../resources/resource_manager.hpp"
#include "location.hpp"

class Campaign : public sf::Drawable
{
	private:
		bool loaded = false;
		std::string title;
		std::string description;
		std::string startLocation;
		std::string worldMapBackgroundId;
		uint currentLocationIdx = 0;
		std::vector<Location> locations;

	public:
		bool load(std::string campaignDir, ResourceManager &resMgr);
		void unload(ResourceManager &resMgr);
		std::string getTitle();
		std::string getDescription();
		std::string getWorldMapBackground();
		std::vector<Location>& getLocations();
		uint getCurrentLocationIdx();
		bool changeLocation(uint locIdx);
		bool changeLocationById(std::string locId);
		bool isLoaded();
		bool gotoRoom(Direction direction);
		sf::Vector2u getPlayerRoomCoords();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
