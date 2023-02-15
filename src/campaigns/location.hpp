#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/System/Clock.hpp>

#include "../materials/material_manager.hpp"
#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"
#include "../objects/object_manager.hpp"
#include "../util/json.hpp"
#include "../consts.hpp"
#include "room_grid.hpp"

#define REC_LVL_EMPTY -1

/**
 * Location represents a collection of Rooms connected to each other. Player is able to move between Rooms belonging to
 * the same Location.
 *
 * The Location class has three stages of being loaded, each stage allows the next to be executed:
 *   1. The object was created
 *   2. Metadata was loaded with ::loadMeta()
 *   3. Content was loaded with ::loadContent()
 * Only in 3rd stage (after loading content), the Location is usable (the player can travel to it).
 * Location should load its metadata once, then its content can be loaded and unloaded as many times as it is necessary,
 * in order to save memory when the Location is not being actively used.
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
 * Location is responsible for animating room transition. For this purpose, it uses the internal state flag
 * ::roomTransitionInProgress (separate from global GameState). When room change is initiated (via ::gotoRoom()),
 * the old room, along with new room, are both rendered into a caching texture (::roomTransitionTxt), which is displayed
 * instead of room during the transition. The caching texture is moved during the animation to create a linear
 * transition effect. When the animation finishes, Location returns to displaying the current room. During the
 * animation, the simulation state is not being updated.
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
		Player &player;

		// room transition is *not* another GameState (see ::gameState in main), but rather an internal state of
		// Location. from main's perspective, the state could be still STATE_PLAYING, but the Location, instead of
		// actually continuing simulation (via ::tick()), will be in the process of room transition and will act
		// as if the simulation was paused (for the time of transition).
		bool roomTransitionInProgress = false;
		enum Direction roomTransitionDirection;
		uint roomTransitionOffset = 0;
		sf::Clock roomTransitionTimer;
		sf::Texture roomTransitionTxt;
		sf::Sprite roomTransitionSprite;

		bool validateRoomGeometry(const std::shared_ptr<Room> room, HashableVector3i roomCoords) const;

	public:
		Location(const std::string &id, Player &player);
		bool loadMeta(const json &locMetaNode, const std::string &campaignDir);
		bool loadContent(ResourceManager &resMgr, const MaterialManager &matMgr, const ObjectManager &objMgr);
		void unloadContent();
		std::string getId() const;
		std::string getTitle() const;
		std::string getDescription() const;
		sf::Vector2u getWorldMapCoords() const;
		bool isWorldMapIconBig() const;
		bool isBasecamp() const;
		uint getRecommendedLevel() const;
		std::string getWorldMapIconId() const;
		bool gotoRoom(Direction direction, sf::Vector2f newPlayerCoords);
		bool gotoRoom(HashableVector3i coords);
		void redraw();
		sf::Vector3i getPlayerRoomCoords() const;
		sf::Vector2u getSpawnCoords() const;
		void tick(uint lastFrameDurationUs);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
