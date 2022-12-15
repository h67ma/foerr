#pragma once

#include <string>
#include <SFML/Graphics/Drawable.hpp>
#include "../util/json.hpp"
#include "room_cell.hpp"

#define ROOM_WIDTH_WITH_BORDER 48
#define ROOM_HEIGHT_WITH_BORDER 25

// TODO rooms CAN and WILL appear multiple times per location, especially in grind
// locations, so Room objects would either have to be cloned (easy), or we'd need to
// create some kind of RoomInstance class or whatever

/**
 * Room class is a representation of a part of a location that fits on a single screen.
 */
class Room : public sf::Drawable
{
	private:
		RoomCell cells[ROOM_HEIGHT_WITH_BORDER][ROOM_WIDTH_WITH_BORDER];
		// TODO void flip(); // for mirroring room vertically, only for grind maps

	public:
		bool load(const json &root, const std::string &filePath);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
