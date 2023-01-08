#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include "../util/json.hpp"
#include "../materials/material_manager.hpp"
#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"
#include "../objects/object_manager.hpp"
#include "room_cell.hpp"

#define ROOM_WIDTH_WITH_BORDER 48
#define ROOM_HEIGHT_WITH_BORDER 25

// that's the worst name ever for a struct
struct blend_sprite
{
	SpriteResource spriteRes;
	bool blend;
};

/**
 * Room is a representation of a part of a location that fits on a single screen.
 *
 * TODO rooms CAN and WILL appear multiple times per location in grind locations, so it would be good to clone the
 * already loaded rooms when loading duplicate rooms, to avoid repeating the complicated loading process. Of course
 * the clone will need to go through rest of the setup separately (e.g. spawning objects).
 */
class Room : public sf::Drawable, public sf::Transformable
{
	private:
		RoomCell cells[ROOM_HEIGHT_WITH_BORDER][ROOM_WIDTH_WITH_BORDER];
		SpriteResource backwall;
		SpriteResource liquidDelim;
		sf::RectangleShape liquid;
		sf::Texture backCacheTxt;
		sf::Texture frontCacheTxt;
		sf::Sprite backCache; // immutable elements - background, room backwall, background objects
		sf::Sprite frontCache; // mutable elements - solids, ladders, stairs, platforms, liquids
		uint liquidLevelHeight;
		sf::Texture cachedLiquidLevelTxt;
		sf::Sprite cachedLiquidLevel;
		sf::Vector2u spawnCoords;
		sf::CircleShape dummyPlayerSpawn; // TODO delet this
		std::vector<SpriteResource> backObjectsMain;
		std::vector<SpriteResource> farBackObjectsMain;
		std::vector<struct blend_sprite> backHoleObjectsMain;
		std::vector<SpriteResource> backHoleObjectsHoles;

		// TODO void flip(); // for mirroring room vertically, only for grind maps. here "is_right" will become useful
		bool parseBackObjsNode(const json &root, const std::string &filePath, ResourceManager &resMgr,
							   const ObjectManager &objMgr, const char *key, bool far);
		bool parseBackHoleObjsNode(const json &root, const std::string &filePath, ResourceManager &resMgr,
								   const ObjectManager &objMgr);

	public:
		bool load(ResourceManager &resMgr, const MaterialManager &matMgr, const ObjectManager &objMgr,
				  const json &root, const std::string &filePath);
		void init();
		void deinit();
		void redrawCell(uint x, uint y, sf::RenderTarget &target, sf::RenderStates states) const; // TODO use me
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
