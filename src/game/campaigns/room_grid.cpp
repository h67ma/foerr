#include "room_grid.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

/**
 * Sets the grid dimensions and sizes the internal vector accordingly.
 * Old vector contents are removed.
 *
 * @param width new grid width (in rooms)
 * @param height new grid height (in rooms)
 */
void RoomGrid::setDimens(uint width, uint height)
{
	this->width = width;
	this->height = height;
	this->grid.clear(); // just in case
	this->grid.resize(width * height);
}

/**
 * Sets the room at (x, y) to a Room object.
 * If an object is already assigned, it will be deallocated.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param room pointer to the Room object to set
 */
bool RoomGrid::set(uint x, uint y, Room *room)
{
	if (x >= this->width || y >= this->height)
	{
		Log::e(STR_IDX_OUTTA_BOUNDS);
		return false;
	}

	this->grid[y * this->width + x] = std::unique_ptr<Room>(room);
	return true;
}

/**
 * Gets the room at (x, y).
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @returns a pointer to the Room object at (x, y). If the coordinates are invalid, or contain an empty room, `nullptr` is returned.
 */
Room* RoomGrid::get(uint x, uint y)
{
	if (x >= this->width || y >= this->height)
	{
		Log::e(STR_IDX_OUTTA_BOUNDS);
		return nullptr;
	}

	return this->grid[y * this->width + x].get();
}

void RoomGrid::clear()
{
	this->grid.clear();
	this->width = 0;
	this->height = 0;
}
