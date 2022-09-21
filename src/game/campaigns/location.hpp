#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "room_grid.hpp"
#include "../consts.hpp"
#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"

/**
 * Location file structure:
 * {
 *	"title": "Location Name",
 *	"description": "Description shown on world map page.",
 *	"grind": false,
 *	"basecamp": false,
 *	"background_full": "path/to/img.png",	// optional
 *	"worldmap_icon": "res/campaigns/test/hud/icons/surface.png",
 *	"worldmap_x": 123,
 *	"worldmap_y": 456,
 *	"worldmap_icon_big": false,	// optional
 *	"width": 2,
 *	"height": 3,
 *	"start_x": 0,	// optional
 *	"start_y": 0,	// optional
 *	"room_map": [
 *		["one",		"two"],
 *		["three",	"/EMPTY"],
 *		["five",	"six"]
 *	]
 * }
 *
 * TODO inherit UniqueLocation and GeneratedLocation
 */
class Location : public sf::Drawable
{
	private:
		const std::string id;
		const std::string locPath;
		std::string title;
		std::string description;
		uint worldMapX;
		uint worldMapY;
		bool isWorldMapIconBig = false;
		std::string worldMapIconId;
		bool isGrind;
		bool isBasecamp;
		SpriteResource backgroundFullSprite;
		RoomGrid rooms;
		sf::Vector2u playerRoomCoords = { 0, 0 };
		std::shared_ptr<Room> currentRoom = nullptr;

	public:
		Location(std::string id, std::string locPath);
		bool loadMeta();
		bool loadContent(ResourceManager &resMgr);
		void unloadContent();
		std::string getId();
		std::string getTitle();
		std::string getDescription();
		uint getWorldMapX();
		uint getWorldMapY();
		bool getIsWorldMapIconBig(); // gee, that's a mouthful
		bool getIsBasecamp();
		std::string getWorldMapIconId();
		bool gotoRoom(Direction direction);
		sf::Vector2u getPlayerRoomCoords();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
