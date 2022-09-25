#include "room_grid.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

/**
 * Sets the grid dimensions and sizes the internal vector accordingly.
 * Old grid contents are removed.
 *
 * @param dimens new grid size
 */
void RoomGrid::setDimens(sf::Vector2u dimens)
{
	this->dimens = dimens;
	this->grid.clear(); // just in case
	this->grid.resize(dimens.x * dimens.y);
}

/**
 * Gets current room coordinates in the room grid.
 *
 * @return sf::Vector2u current room coordinates
 */
sf::Vector2u RoomGrid::getCurrentCoords()
{
	return this->currentCoords;
}

/**
 * Sets the room at specified coordinates to a Room object pointer.
 * If an object is already assigned, it will be deallocated.
 *
 * @param coords the room coordinates
 * @param room shared pointer to the Room object to set
 * @return true if set was successful
 * @return false if set failed
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
 * Gets the room at specified coordinates.
 *
 * @param coords room coordinates
 * @return a shared pointer to the Room object at specified coords, if it exists
 * @return nullptr, if the coordinates are invalid, or contain an empty room
 */
std::shared_ptr<Room> RoomGrid::get(sf::Vector2u coords)
{
	if (coords.x >= this->dimens.x || coords.y >= this->dimens.y)
		return nullptr;

	return this->grid[coords.y * this->dimens.x + coords.x];
}

/**
 * Checks if a room at specified coordinates exists, and if so,
 * sets current coordinates to it and returns it.
 *
 * @param coords coordinates to move to
 * @return a shared pointer to the Room object at specified coords, if it exists
 * @return nullptr, if the coordinates are invalid, or contain an empty room
 */
std::shared_ptr<Room> RoomGrid::moveTo(sf::Vector2u coords)
{
	std::shared_ptr<Room> found = this->get(coords);
	if (found != nullptr)
		this->currentCoords = coords;

	return found;
}

/**
 * Checks if a room near the current one exists and returns its coordinates if it does.
 *
 * @param direction direction to search from current room
 * @param coords reference to coordinates of nearby room, valid only if return value is true
 * @return true if newCoords are valid
 * @return false if newCoords are not valid
 */
bool RoomGrid::nearExists(Direction direction, sf::Vector2u &newCoords)
{
	newCoords = this->currentCoords;

	if (direction == DIR_UP)
	{
		if (newCoords.y == 0)
			return false; // can't go any higher

		newCoords.y -= 1;
	}
	else if (direction == DIR_LEFT)
	{
		if (newCoords.x == 0)
			return false; // can't go any more left

		newCoords.x -= 1;
	}
	else if (direction == DIR_DOWN)
	{
		newCoords.y += 1;
	}
	else if (direction == DIR_RIGHT)
	{
		newCoords.x += 1;
	}

	if (this->get(newCoords) == nullptr)
		// the system you're searching for doesn't exist.
		// impossible! perhaps the archives are incomplete?
		return false;

	newCoords = newCoords;
	return true;
}

/**
 * Finds the room in the specified direction from current one and if it exists
 * and sets current coordinates to it.
 *
 * @param direction direction to search from current room
 * @return shared pointer to new room, if it exists
 * @return nullptr, if new room does not exist
 */
std::shared_ptr<Room> RoomGrid::moveToNear(Direction direction)
{
	sf::Vector2u newCoords;
	if (!this->nearExists(direction, newCoords))
		return nullptr;

	this->currentCoords = newCoords;
	return this->get(newCoords);
}

void RoomGrid::clear()
{
	this->grid.clear();
	this->dimens = { 0, 0 };
}
