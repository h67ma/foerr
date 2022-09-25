#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "room_grid.hpp"
#include "../consts.hpp"
#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"

#define REC_LVL_EMPTY -1

/**
 * Location file structure:
 * {
 *	"title": "Location Name",
 *	"description": "Description shown on world map page.",
 *	"grind": false,
 *	"basecamp": false,
 *	"rec_lvl": 42,	// optional
 *	"background_full": "path/to/img.png",	// optional
 *	"worldmap_icon": "res/campaigns/test/hud/icons/surface.png",
 *	"worldmap_icon_big": false,	// optional
 *	"worldmap_coords": [123, 456],
 *	"rooms": [
 *		...	// see room.cpp
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
		sf::Vector2u worldMapCoords;
		bool isWorldMapIconBig = false;
		std::string worldMapIconId;
		bool isGrind;
		bool isBasecamp;
		uint recommendedLevel = REC_LVL_EMPTY;
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
		sf::Vector2u getWorldMapCoords();
		bool getIsWorldMapIconBig(); // gee, that's a mouthful
		bool getIsBasecamp();
		uint getRecommendedLevel();
		std::string getWorldMapIconId();
		bool gotoRoom(Direction direction);
		sf::Vector2u getPlayerRoomCoords();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
