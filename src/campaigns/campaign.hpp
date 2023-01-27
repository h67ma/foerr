#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include <SFML/System/Vector3.hpp>

#include "../resources/resource_manager.hpp"
#include "../materials/material_manager.hpp"
#include "../objects/object_manager.hpp"
#include "../entities/player.hpp"
#include "location.hpp"

/**
 * The Campaign class stores Locations and other useful information related to the currently loaded campaign.
 *
 * Metadata for all Locations is kept loaded, but not all Location content is kept loaded at the same time. Apart from
 * the current Location, all basecamps visited since the Campaign was created are kept fully loaded, as well as the
 * previous non-basecamp Location, if the player traveled from it to a basecamp.
 */
class Campaign : public sf::Drawable
{
	private:
		Player player;
		ResourceManager &resMgr;
		MaterialManager matMgr;
		ObjectManager objMgr;
		std::string title;
		std::string description;

		// could be hardcoded, but it would prevent reusing images from other campaigns
		std::string worldMapBackgroundId;

		std::shared_ptr<Location> currentLocation = nullptr;

		// last visited non-basecamp location which should be unloaded when traveling to a new non-basecamp location
		std::shared_ptr<Location> lastUnloadableLocation = nullptr;

		std::unordered_map<std::string, std::shared_ptr<Location>> locations;

	public:
		explicit Campaign(ResourceManager &resMgr);
		bool load(const std::string &campaignDir);
		void unload();
		std::string getTitle() const;
		std::string getDescription() const;
		std::string getWorldMapBackground() const;
		const std::unordered_map<std::string, std::shared_ptr<Location>>& getLocations() const;
		const std::shared_ptr<Location> getCurrentLocation() const;
		const std::shared_ptr<Location> getLocation(const std::string &locId) const;
		bool changeLocation(const std::string &newLocId);
		bool isLoaded();
		bool gotoRoom(Direction direction);
		bool gotoRoom(HashableVector3i coords);
		void redraw();
		void logWhereAmI();
		void updateState();
		void nextFrame();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
