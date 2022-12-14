#pragma once

/**
 * RoomCell is the basic building block of all Rooms. It represents a static (unmovable), square area, roughly half the
 * height/width of the player in size. In that area, multiple elements can be present at the same time, including:
 *   - Background - only changes how the Room's "back wall" looks. Because each cell can define a separate background,
 *					smaller areas can be divided inside Room to represent rooms, corridors, outdoors, etc.
 *   - Solid - a surface, which interacts with physical entities (objects/items/player/enemies/NPCs/projectiles). Most
 *					solids can be damaged and eventually destroyed, leaving empty (physical) space (background will
 *					remain). Each solid material defines how tough it is (can be unbreakable). Other than that, all
 *					solids behave essentially the same. Additionally, the height of a solid can be controlled by the
 *					part-height directive, allowing solids to have 5 height levels: 0 (no solid), 1/4, 1/2, 3/4, and 1.
 *					Apart from visual change, in certain cases this allows alive entities (player/enemies/NPCs) to
 *					traverse a slope made of part-height solids without jumping.
 *   - Platform - similar to solid, but only acts as a physical surface from the top side. It will not limit movement
 *					of physical entities when they enter the cell area from the sides or from the bottom. Additionally,
 *					alive entities can choose to drop down from the platform.
 *   - Stairs - similar to platform, but the top surface is at 45 degree angle.
 *   - Ladder - does not limit movement of physical entities from any side, but alive entities can climb it, either up
 *					or down.
 *   - Liquid - the cell is flooded with water/radioactive goo/pink cloud water. The liquid can be drained by breaking
 *					nearby solids.
 *
 * Each element type can appear only once in a cell (e.g. there can't be two backgrounds defined). There are also other
 * restrictions (see ::addSolidSymbol() and ::addOtherSymbol() for details).
 */
class RoomCell
{
	public:
		bool addSolidSymbol(char symbol);
		bool addOtherSymbol(char symbol);
		bool validate() const;
};