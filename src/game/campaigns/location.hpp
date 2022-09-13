#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "room_grid.hpp"
#include "../consts.hpp"
#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"

// TODO? maybe inherit UniqueLocation and GeneratedLocation
class Location : public sf::Drawable
{
	private:
		std::string id;
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
		sf::Vector2u startCoords;

	public:
		Location(std::string id);
		bool load(std::string locPath, ResourceManager &resMgr);
		std::string getId();
		std::string getTitle();
		std::string getDescription();
		uint getWorldMapX();
		uint getWorldMapY();
		bool getIsWorldMapIconBig(); // gee, that's a mouthful
		bool getIsBasecamp();
		std::string getWorldMapIconId();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
