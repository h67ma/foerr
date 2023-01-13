#include "room_grid.hpp"

/**
 * Gets the current room coordinates.
 *
 * @return current room coordinates
 */
HashableVector3i RoomGrid::getCurrentCoords() const
{
	return this->currentCoords;
}

/**
 * Creates a new room at specified coordinates.
 * If another room already exists at this coordinates, it will be overwritten without any warning
 * (this case should be handled by the caller, see Location::loadContent()).
 *
 * @param coords the room coordinates
 * @param room shared pointer to the Room object to set
 */
void RoomGrid::set(HashableVector3i coords, std::shared_ptr<Room> room)
{
	// emplacing on existing idx removes the old element, and because it's a smart ptr, it will be deallocated
	this->grid.emplace(coords, room);
}

/**
 * Gets the room at specified coordinates.
 *
 * @param coords room coordinates
 * @return a shared pointer to the Room object at specified coords, if it exists
 * @return nullptr, if the coordinates are invalid, or contain an empty room
 */
std::shared_ptr<Room> RoomGrid::get(HashableVector3i coords) const
{
	auto search = this->grid.find(coords);
	if (search == this->grid.end())
		// "The system you're searching for doesn't exist."
		// "Impossible! Perhaps the archives are incomplete?"
		return nullptr;

	return search->second;
}

/**
 * Checks if a room at specified coordinates exists, and if so,
 * sets current coordinates to it and returns it.
 *
 * @param coords coordinates to move to
 * @return a shared pointer to the Room object at specified coords, if it exists
 * @return nullptr, if the coordinates are invalid, or contain an empty room
 */
std::shared_ptr<Room> RoomGrid::moveTo(HashableVector3i coords)
{
	std::shared_ptr<Room> found = this->get(coords);
	if (found != nullptr)
		this->currentCoords = coords;

	return found;
}

/**
 * Finds the room in the specified direction from current one and sets it as current coordinates if it exists.
 *
 * @param direction direction to search from current room
 * @return shared pointer to new room, if it exists
 * @return nullptr, if new room does not exist
 */
std::shared_ptr<Room> RoomGrid::moveToNear(Direction direction)
{
	HashableVector3i newCoords = this->currentCoords;

	switch (direction)
	{
		case DIR_LEFT:
			newCoords.x -= 1;
			break;
		case DIR_RIGHT:
			newCoords.x += 1;
			break;
		case DIR_UP:
			newCoords.y -= 1;
			break;
		case DIR_DOWN:
			newCoords.y += 1;
			break;
		case DIR_FRONT:
			newCoords.z -= 1;
			break;
		case DIR_BACK:
			newCoords.z += 1;
			break;
		default:
			break; // a horrible chill goes down your spine...
	}

	std::shared_ptr<Room> newRoom = this->get(newCoords);
	if (newRoom == nullptr)
		return nullptr;

	this->currentCoords = newCoords;
	return newRoom;
}

void RoomGrid::clear()
{
	this->grid.clear();
}
