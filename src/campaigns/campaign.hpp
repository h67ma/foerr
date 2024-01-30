// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <SFML/System/Vector3.hpp>

#include "../entities/player.hpp"
#include "../materials/material_manager.hpp"
#include "../objects/object_manager.hpp"
#include "../resources/resource_manager.hpp"
#include "../settings/keymap.hpp"
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
		std::string id;
		std::string title;
		std::string description;
		Player player;
		ResourceManager& resMgr;
		MaterialManager matMgr;
		ObjectManager objMgr;

		std::shared_ptr<Location> currentLocation = nullptr;

		// last visited non-basecamp location which should be unloaded when traveling to a new non-basecamp location
		std::shared_ptr<Location> lastUnloadableLocation = nullptr;

		std::unordered_map<std::string, std::shared_ptr<Location>> locations;

	public:
		explicit Campaign(ResourceManager& resMgr);
		bool load(const std::string& campaignId);
		void unload();
		std::string getId() const;
		std::string getTitle() const;
		std::string getDescription() const;
		Player& getPlayer();
		const std::unordered_map<std::string, std::shared_ptr<Location>>& getLocations() const;
		const std::shared_ptr<Location> getCurrentLocation() const;
		const std::shared_ptr<Location> getLocation(const std::string& locId) const;
		bool changeLocation(const std::string& newLocId);
		bool isLoaded();
		bool gotoRoom(Direction direction);
		bool gotoRoom(HashableVector3i coords);
		void redraw();
		void logWhereAmI();
		void tick(uint lastFrameDurationUs);
		void nextFrame();
		void teleportPlayer(sf::Vector2f position);
		void rerollObjVariants();
		void setRoomLightsState(enum LightObjectsState state);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
