#pragma once

#include <vector>
#include "room.hpp"
#include "../consts.hpp"

/**
 * RoomGrid is used to store rooms for a single location, organized in a 2d grid.
 */
class RoomGrid
{
	private:
		uint width; // number of rooms horizontally (including empty)
		uint height; // number of rooms vertically (including empty)
		std::vector<std::shared_ptr<Room>> grid; // grid of rooms, a fake 2d array

	public:
		void setDimens(uint width, uint height);
		bool set(uint x, uint y, std::shared_ptr<Room> room);
		Room* get(sf::Vector2u coords);
		void clear();
};
