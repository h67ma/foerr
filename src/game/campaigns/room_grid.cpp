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
void RoomGrid::setDimens(sf::Vector2u dimens)
{
	this->dimens = dimens;
	this->grid.clear(); // just in case
	this->grid.resize(dimens.x * dimens.y);
}

/**
 * Sets the room at (x, y) to a Room object.
 * If an object is already assigned, it will be deallocated.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param room pointer to the Room object to set
 */
bool RoomGrid::set(sf::Vector2u coords, std::shared_ptr<Room> room)
{
	if (coords.x >= this->dimens.x || coords.y >= this->dimens.y)
	{
		Log::e(STR_IDX_OUTTA_BOUNDS);
		return false;
	}

	this->grid[coords.y * this->dimens.x + coords.x] = room;
	return true;
}

/**
 * Gets the room at (x, y).
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @returns a pointer to the Room object at (x, y). If the coordinates are invalid, or contain an empty room, `nullptr` is returned.
 */
std::shared_ptr<Room> RoomGrid::get(sf::Vector2u coords)
{
	if (coords.x >= this->dimens.x || coords.y >= this->dimens.y)
		return nullptr;

	return this->grid[coords.y * this->dimens.x + coords.x];
}

void RoomGrid::clear()
{
	this->grid.clear();
	this->dimens = { 0, 0 };
}
