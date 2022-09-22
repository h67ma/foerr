#pragma once

#include <vector>
#include <SFML/System/Vector2.hpp>
#include "room.hpp"
#include "../consts.hpp"

/**
 * "The Grid. A digital frontier..."
 * RoomGrid is used to store rooms for a single location, organized in a 2d grid.
 */
class RoomGrid
{
	private:
		sf::Vector2u dimens; // number of rooms, including empty
		std::vector<std::shared_ptr<Room>> grid; // grid of rooms, a fake 2d array

	public:
		void setDimens(sf::Vector2u dimens);
		bool set(sf::Vector2u coords, std::shared_ptr<Room> room);
		std::shared_ptr<Room> get(sf::Vector2u coords);
		void clear();
};
