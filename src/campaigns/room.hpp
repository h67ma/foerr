#pragma once

#include <string>
#include <SFML/Graphics/Drawable.hpp>
#include "../util/json.hpp"

#define ROOM_WIDTH_WITH_BORDER 48
#define ROOM_HEIGHT_WITH_BORDER 25

#define ROOM_CHARS_PER_CELL 4 // 2x background, 1x block, 1x ROOM_CHAR_SEPARATOR
#define ROOM_LINE_WIDTH_WITH_BORDER (ROOM_WIDTH_WITH_BORDER * ROOM_CHARS_PER_CELL) - 1 // "abc|d_f|_hi"

#define ROOM_SYMBOL_SEPARATOR '|'
#define ROOM_SYMBOL_EMPTY '_'

// TODO rooms CAN and WILL appear multiple times per location, especially in grind
// locations, so Room objects would either have to be cloned (easy), or we'd need to
// create some kind of RoomInstance class or whatever

/**
 * Room class is a representation of a part of a location that fits on a single screen.
 */
class Room : sf::Drawable
{
	private:
		bool drawBackgroundFull = true;
		char backgrounds[ROOM_HEIGHT_WITH_BORDER][ROOM_WIDTH_WITH_BORDER];
		char backgrounds2[ROOM_HEIGHT_WITH_BORDER][ROOM_WIDTH_WITH_BORDER];
		char cells[ROOM_HEIGHT_WITH_BORDER][ROOM_WIDTH_WITH_BORDER];
		// TODO void flip(); // for mirroring room vertically, only for grind maps

	public:
		bool shouldDrawBackgroundFull();
		bool load(const json &root, const std::string &filePath);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
