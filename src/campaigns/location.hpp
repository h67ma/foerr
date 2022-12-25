#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector3.hpp>
#include "room_grid.hpp"
#include "../consts.hpp"
#include "../util/json.hpp"
#include "../materials/material_manager.hpp"
#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"

#define REC_LVL_EMPTY -1

/**
 * Location represents a collection of Rooms connected to each other. Player is able to move between Rooms belonging to
 * the same Location.
 *
 * Drawing a particular Room is a complicated and expensive process, so measures are taken in order to optimize it.
 *
 * First, only one Room is shown at any time (except transitions). This means that we should always only draw a single
 * room (the one where player is currently located). The rest of the rooms are kept loaded and ready to be drawn - they
 * keep information about their cells, objects, and enemies/NPCs (not only initial state, but the state after the player
 * has changed anything in the Room, e.g. broke a wall). The Rooms (or more specifically their cells) also keep pointers
 * to textures needed to draw the Room. This means that all required textures are being loaded when loading the
 * Location, NOT when switching to a particular Room, to minimize the delay when switching Rooms.
 *
 * Second, result of drawing static Room elements (cells and big static elements) is cached in a texture
 * (::roomStaticTxt). Location keeps this texture, on which the current Room is rendered via ::draw(). The whole Room is
 * rendered only once, when entering the Room, and then the cached texture is displayed on each frame. We can do that,
 * because static elements rarely change appearance, so on each frame most of them would have been drawn exactly the
 * same. When a cell (or other static element) is damaged/destroyed/etc, Room's ::redrawCell() is called and only
 * redraws the cell that changed on the cached texture.
 *
 * TODO? the current concept of loading resources could potentially be optimized memory-wise, i.e. we could preload
 * resources only for nearby Rooms, instead of all Rooms in the Location. This would have to be done in the background.
 *
 * TODO inherit UniqueLocation and GeneratedLocation
 */
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
		sf::Texture roomStaticTxt;
		sf::Sprite roomStatic;

	public:
		explicit Location(std::string id);
		bool loadMeta(const json &locMetaNode, const std::string &campaignDir);
		bool loadContent(ResourceManager &resMgr, const MaterialManager &matMgr);
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
		sf::Vector3i getPlayerRoomCoords();
		void preRenderRoomStatic();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
