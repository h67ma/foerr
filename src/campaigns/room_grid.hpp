#pragma once

#include <vector>
#include <memory>
#include "room.hpp"
#include "../consts.hpp"

/**
 * "The Grid. A digital frontier..."
 *
 * RoomGrid is used to store rooms for a single location, organized in a 3d grid.
 *
 * The third dimension is needed to store backrooms. While it could be stored in a boolean (coords would be represented
 * by a struct of Vector2u + bool), storing all three in a single vector is convenient and also provides the possibility
 * of adding more Z layers. While having more than two Z layers is confusing, it may make sense for some locations.
 *
 * TODO? rooms could also be stored in a regular vector and themselves store pointers to nearby rooms. This would make
 * pre-sizing the grid not necessary, potentialy reducing the complexity of Location::loadContent(), and also make
 * storing sparse locations more memory-efficient, though not by much, as we store pointers rather than objects. It
 * would also make RoomGrid mostly unnecessary.
 */
class RoomGrid
{
	private:
		Vector3u currentCoords = { 0, 0, 0 };
		Vector3u dimens; // max number of rooms in all dimensions
		std::vector<std::shared_ptr<Room>> grid; // grid of rooms, a fake 3d array

	public:
		void setDimens(Vector3u dimens);
		Vector3u getCurrentCoords();
		bool set(Vector3u coords, std::shared_ptr<Room> room);
		std::shared_ptr<Room> get(Vector3u coords);
		std::shared_ptr<Room> moveTo(Vector3u coords);
		std::shared_ptr<Room> moveToNear(Direction direction);
		void clear();
};
