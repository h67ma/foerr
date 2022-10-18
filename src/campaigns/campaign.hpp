#pragma once

#include <vector>
#include "../resources/resource_manager.hpp"
#include "location.hpp"

/**
 * Not all locations are kept loaded at the same time. Apart from the
 * current location, all basecamps visited since the program start are
 * kept loaded, as well as previous non-basecamp location, if player
 * travelled from it to a basecamp.
 */
class Campaign : public sf::Drawable
{
	private:
		ResourceManager &resMgr;
		bool loaded = false;
		std::string title;
		std::string description;
		std::string startLocation;
		std::string worldMapBackgroundId;
		uint currentLocationIdx = -1;
		std::vector<Location> locations;
		void unloadSomeLocations(uint newIdx);

	public:
		Campaign(ResourceManager &resMgr);
		bool load(std::string campaignDir);
		void unload();
		std::string getTitle();
		std::string getDescription();
		std::string getWorldMapBackground();
		std::vector<Location>& getLocations();
		uint getCurrentLocationIdx();
		bool changeLocationByIndex(uint newIdx);
		bool changeLocationById(std::string locId);
		bool isLoaded();
		bool gotoRoom(Direction direction);
		sf::Vector2u getPlayerRoomCoords();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};