#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <SFML/Graphics/Drawable.hpp>
#include "room_grid.hpp"
#include "../consts.hpp"
#include "../util/json.hpp"
#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"

#define REC_LVL_EMPTY -1

// TODO inherit UniqueLocation and GeneratedLocation
class Location : public sf::Drawable
{
	private:
		const std::string id;
		std::string roomDataPath;
		std::string title;
		std::string description;
		sf::Vector2u worldMapCoords;
		std::string worldMapIconId;
		bool worldMapIconBig = false;
		bool grind;
		bool basecamp;
		uint recommendedLevel = REC_LVL_EMPTY;
		SpriteResource backgroundFullSprite;
		RoomGrid rooms;
		std::shared_ptr<Room> currentRoom = nullptr;

	public:
		explicit Location(std::string id);
		bool loadMeta(const json &locMetaNode, const std::string &campaignDir);
		bool loadContent(ResourceManager &resMgr);
		void unloadContent();
		std::string getId();
		std::string getTitle();
		std::string getDescription();
		sf::Vector2u getWorldMapCoords();
		bool isWorldMapIconBig();
		bool isBasecamp();
		uint getRecommendedLevel();
		std::string getWorldMapIconId();
		bool gotoRoom(Direction direction);
		Vector3u getPlayerRoomCoords();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
