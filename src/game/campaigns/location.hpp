#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "room_grid.hpp"
#include "../consts.hpp"
#include "../resource_manager.hpp"

// TODO? maybe inherit UniqueLocation and GeneratedLocation
class Location : public sf::Drawable
{
	private:
		std::string id;
		std::string title;
		std::string description;
		uint worldMapX;
		uint worldMapY;
		std::string worldMapIconId;
		bool isGrind;
		bool isBasecamp;
		sf::Sprite backgroundFullSprite;
		RoomGrid rooms;

	public:
		Location(std::string id);
		bool load(std::string locDir, ResourceManager &resMgr);
		std::string getId();
		std::string getTitle();
		std::string getDescription();
		uint getWorldMapX();
		uint getWorldMapY();
		std::string getWorldMapIconId();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
