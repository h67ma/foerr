#pragma once

#include <unordered_map>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "../materials/material_manager.hpp"
#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"

#define CELL_SIDE_LEN 40U

// TODO figure out the exact value (but looks about right)
#define BACKWALL_COLOR COLOR_GRAY(80)

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
class RoomCell : public sf::Transformable
{
	private:
		SpriteResource solid;
		SpriteResource solidMask;
		SpriteResource background;
		SpriteResource platform;
		SpriteResource stairs;
		SpriteResource ladder;
		SpriteResource ladderDelim;
		SpriteResource liquidDelim;
		sf::RectangleShape liquid{ sf::Vector2f(CELL_SIDE_LEN, CELL_SIDE_LEN) };
		int topOffset = 0; // offset from top of cell area, used to create part-height cells
		bool topCellBlocksLadderDelim;
		bool topCellBlocksLiquidDelim;
		static const std::unordered_map<char, int> heightFlags;

		// all these flags could potentially be moved to a single uint to save a bit of memory, but it seems that the
		// compiler is already doing a similar optimization by itself. it also makes the code less readable, so let's
		// skip it for now.
		bool hasSolid = false;
		bool hasBackground = false;
		bool hasPlatform = false;
		bool hasStairs = false;
		bool hasLadder = false;
		bool hasLiquid = false;

	public:
		static const sf::Color liquidSpriteColor;
		bool addSolidSymbol(char symbol, ResourceManager &resMgr, const MaterialManager &matMgr);
		bool addOtherSymbol(char symbol, bool topCellBlocksLadderDelim, bool topCellBlocksLiquidDelim,
							ResourceManager &resMgr, const MaterialManager &matMgr);
		bool finishSetup();
		bool blocksBottomCellLadderDelim() const;
		bool blocksBottomCellLiquidDelim() const;
		bool getHasSolid() const;
		void drawBackground(sf::RenderTarget &target) const;
		void drawPlatform(sf::RenderTarget &target) const;
		void drawStairs(sf::RenderTarget &target) const;
		void drawLadder(sf::RenderTarget &target) const;
		void drawLiquidAndSolid(sf::RenderTarget &target) const;
};
