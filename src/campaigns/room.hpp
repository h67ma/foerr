#pragma once

#include <string>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "../util/json.hpp"
#include "../materials/material_manager.hpp"
#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"
#include "room_cell.hpp"

#define ROOM_WIDTH_WITH_BORDER 48
#define ROOM_HEIGHT_WITH_BORDER 25

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
		sf::RectangleShape liquid;
		sf::Texture cachedCellsTxt;
		sf::Sprite cachedCells;
		// TODO void flip(); // for mirroring room vertically, only for grind maps. here "is_right" will become useful

	public:
		bool load(ResourceManager &resMgr, const MaterialManager &matMgr, const json &root,
				  const std::string &filePath);
		void init();
		void deinit();
		void redrawCell(uint x, uint y, sf::RenderTarget &target, sf::RenderStates states) const; // TODO use me
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
