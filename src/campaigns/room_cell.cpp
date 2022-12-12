#include "room_cell.hpp"

/**
 * @brief Adds a solid symbol to the cell
 * It is expected that the solid will be added to the cell before any other symbols.
 *
 * Performs checks before adding the symbol to ensure that the cell will be sane. This includes:
 *   1. Material exists and its type is solid
 *   2. No solid has already been added to the cell
 *
 * @param symbol symbol character
 * @return true if the symbol was added successfully
 * @return false if the symbol cannot be added
 */
bool RoomCell::addSolidSymbol(char symbol)
{
	// TODO
	return true;
}

/**
 * @brief Adds a non-solid symbol to the cell
 * It is expected that a solid has already been added to the cell if necessary.
 *
 * Performs checks before adding the symbol to ensure that the cell will be sane. This includes:
 *   1. Material exists and its type is not solid
 *   2. Only one of each symbol type can be added to the cell:
 *     - Solid (covered by ::addSolidSymbol)
 *     - Background
 *     - Platform
 *     - Ladder
 *     - Liquid
 *     - Part-height directive
 *   3. Ladder + solid is an invalid case
 *   4. Stairs + solid is an invalid case
 *   5. Platform + solid is an invalid case
 *   6. Platform + stairs is an invalid case
 *   7. Height flag without solid is an invalid case
 *
 * @param symbol symbol character
 * @return true if the symbol was added successfully
 * @return false if the symbol cannot be added
 */
bool RoomCell::addOtherSymbol(char symbol)
{
	// TODO
	return true;
}

/**
 * @brief Performs santy checks after all symbols have already been added. This includes:
 *   1. Liquid + solid without height flag is an invalid case
 *
 * @return true if the cell is sane
 * @return false if the cell contains errors
 */
bool RoomCell::validate() const
{
	// TODO
	return true;
}
