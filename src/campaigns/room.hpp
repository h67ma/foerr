// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>
#include <vector>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <nlohmann/json.hpp>

#include "../entities/player.hpp"
#include "../materials/material_manager.hpp"
#include "../objects/back_obj_data.hpp"
#include "../objects/object_manager.hpp"
#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"
#include "room_cell.hpp"

constexpr uint ROOM_WIDTH_WITH_BORDER = 48;
constexpr uint ROOM_HEIGHT_WITH_BORDER = 25;

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
		sf::Texture frontCache1Txt;
		sf::Texture frontCache2Txt;
		sf::Sprite backCache; // immutable elements - background, room backwall, background objects
		sf::Sprite frontCache1; // mutable elements behind the Player - stairs, platforms
		sf::Sprite frontCache2; // mutable elements before the Player - solids, ladders, liquids
		uint liquidLevelHeight;
		sf::Texture cachedLiquidLevelTxt;
		sf::Sprite cachedLiquidLevel;
		sf::Vector2u spawnCoords { ROOM_WIDTH_WITH_BORDER / 2, ROOM_HEIGHT_WITH_BORDER / 2 }; // Room center by default
		enum LightObjectsState lightsState;

		std::vector<struct back_obj_data> backObjectsData;
		std::vector<struct back_obj_data> backObjectsDataFar;
		std::vector<struct back_obj_data> backHoleObjectsData;

		std::vector<SpriteResource> backObjectsMain;
		std::vector<SpriteResource> farBackObjectsMain;
		std::vector<struct blend_sprite> backHoleObjectsMain;
		std::vector<SpriteResource> backHoleObjectsHoles;

		Player& player;

		// TODO void flip(); // for mirroring room vertically, only for grind maps. here "is_right" will become useful
		static bool parseBackObjsNode(const nlohmann::json& root, const std::string& filePath, const std::string& key,
									  std::vector<struct back_obj_data>& dataVector);
		void setupBackObjects(ResourceManager& resMgr, const ObjectManager& objMgr,
							  const std::vector<struct back_obj_data>& dataVector,
							  std::vector<SpriteResource>& spriteVector);
		void setupBackHoleObjects(ResourceManager& resMgr, const ObjectManager& objMgr);

	public:
		explicit Room(Player& player);
		bool load(ResourceManager& resMgr, const MaterialManager& matMgr, const ObjectManager& objMgr,
				  const nlohmann::json& root, const std::string& filePath);
		void init();
		void deinit();
		void tick(uint lastFrameDurationUs);
		sf::Vector2u getSpawnCoords() const;
		bool isCellCollider(uint x, uint y) const;
		void setLightsState(enum LightObjectsState state);
		void redrawCell(uint x, uint y, sf::RenderTarget& target, sf::RenderStates states) const; // TODO use me
		void setupAllBackObjects(ResourceManager& resMgr, const ObjectManager& objMgr);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
