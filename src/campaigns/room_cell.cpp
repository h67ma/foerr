// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "room_cell.hpp"

#include <memory>

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "../util/i18n.hpp"
#include "../hud/log.hpp"

const std::unordered_map<char, int> heightFlags {
	{ ',', CELL_SIDE_LEN * 0.25},	// 3/4 height
	{ ';', CELL_SIDE_LEN * 0.5},	// 1/2 height
	{ ':', CELL_SIDE_LEN * 0.75}	// 1/4 height
};

const sf::Color RoomCell::liquidSpriteColor = sf::Color(255, 255, 255, LIQUID_OPACITY);

/**
 * @brief Adds a solid symbol to the cell
 * It is expected that the solid will be added to the cell before any other symbols.
 *
 * Performs checks before adding the symbol to ensure that the cell will be sane. This includes:
 *   1. No solid has already been added to the cell
 *   2. Material exists and its type is solid
 *
 * @param symbol symbol character
 * @param resMgr reference to resource manager
 * @param matMgr reference to material manager
 * @return true if the symbol was added successfully
 * @return false if the symbol cannot be added
 */
bool RoomCell::addSolidSymbol(char symbol, ResourceManager& resMgr, const MaterialManager& matMgr)
{
	// check 1
	if (this->hasSolid)
	{
		Log::e(STR_MAT_SYMBOL_TYPE_ALREADY_PRESENT, symbol);
		return false;
	}

	// check 2
	const struct material *mat = matMgr.getSolid(symbol);
	if (mat == nullptr || mat->type != MAT_SOLID)
	{
		Log::e(STR_MAT_MISSING_OR_WRONG_TYPE, symbol);
		return false;
	}

	std::shared_ptr<sf::Texture> txt = resMgr.getTexture(mat->texturePath);
	if (txt != nullptr)
		txt->setRepeated(true);

	this->solid.setTexture(txt);

	// TODO mask will probably be handled elsewhere
	if (!mat->maskTexturePath.empty())
	{
		txt = resMgr.getTexture(mat->maskTexturePath);
		if (txt != nullptr)
			txt->setRepeated(true);

		this->solidMask.setTexture(txt);
	}

	this->hasSolid = true;
	return true;
}

/**
 * @brief Adds a non-solid symbol to the cell
 * It is expected that a solid has already been added to the cell if necessary.
 *
 * Performs checks before adding the symbol to ensure that the cell will be sane. This includes:
 *   1. Height flag without solid is an invalid case
 *   2. Material exists
 *   3. Material type is not solid
 *   4. Only one of each symbol type can be added to the cell:
 *     - Solid (covered by ::addSolidSymbol)
 *     - Background
 *     - Platform
 *     - Stairs
 *     - Ladder
 *     - Liquid
 *     - Part-height directive
 *   5. Ladder + solid is an invalid case
 *   6. Platform + solid is an invalid case
 *   7. Platform + stairs is an invalid case
 *   8. Stairs + solid is an invalid case
 *
 * @param symbol symbol character
 * @param topCellBlocksLadderDelim whether the cell at (x, y-1) has blocked this cell from drawing ladder delim
 * @param topCellBlocksLiquidDelim whether the cell at (x, y-1) has blocked this cell from drawing water delim (surface)
 * @param resMgr reference to resource manager
 * @param matMgr reference to material manager
 * @return true if the symbol was added successfully
 * @return false if the symbol cannot be added
 */
bool RoomCell::addOtherSymbol(char symbol, bool topCellBlocksLadderDelim, bool topCellBlocksLiquidDelim,
							  ResourceManager& resMgr, const MaterialManager& matMgr)
{
	// first check if symbol is a height flag, as it won't be present in mat mgr
	auto heightFlagSearch = heightFlags.find(symbol);
	if (heightFlagSearch != heightFlags.end())
	{
		// check 1
		if (!this->hasSolid)
		{
			Log::w(STR_HEIGHT_FLAG_NO_SOLID, symbol);
			return false;
		}

		// check 4 (part-height)
		if (this->topOffset == 0)
		{
			this->topOffset = heightFlagSearch->second;
			return true;
		}
		else
		{
			Log::e(STR_HEIGHT_FLAG_ALREADY_PRESENT, symbol);
			return false;
		}
	}

	// check 2
	const struct material *mat = matMgr.getOther(symbol);
	if (mat == nullptr)
	{
		Log::e(STR_MAT_MISSING_OR_WRONG_TYPE, symbol);
		return false;
	}

	if (mat->type == MAT_BG)
	{
		// check 4 (background)
		if (this->hasBackground)
		{
			Log::e(STR_MAT_SYMBOL_TYPE_ALREADY_PRESENT, symbol);
			return false;
		}

		std::shared_ptr<sf::Texture> txt = resMgr.getTexture(mat->texturePath);
		if (txt != nullptr)
			txt->setRepeated(true);

		this->background.setTexture(txt);
		this->background.setTextureRect({
			static_cast<int>(this->getPosition().x),
			static_cast<int>(this->getPosition().y),
			CELL_SIDE_LEN,
			CELL_SIDE_LEN
		});
		this->background.setColor(BACKWALL_COLOR); // darken background

		this->hasBackground = true;
	}
	else if (mat->type == MAT_LADDER)
	{
		// check 4 (ladder)
		if (this->hasLadder)
		{
			Log::e(STR_MAT_SYMBOL_TYPE_ALREADY_PRESENT, symbol);
			return false;
		}

		// check 5
		if (this->hasSolid)
		{
			Log::e(STR_SOLID_PRESENT_CANT_ADD, symbol);
			return false;
		}

		this->ladder.setTexture(resMgr.getTexture(mat->texturePath));
		this->ladder.setPosition({ static_cast<float>(mat->offsetLeft), 0 });

		this->ladderDelim.setTexture(resMgr.getTexture(mat->textureDelimPath));
		this->ladderDelim.setPosition(static_cast<sf::Vector2f>(mat->delimOffset));

		this->topCellBlocksLadderDelim = topCellBlocksLadderDelim;
		this->hasLadder = true;
	}
	else if (mat->type == MAT_PLATFORM)
	{
		// check 4 (platform)
		if (this->hasPlatform)
		{
			Log::e(STR_MAT_SYMBOL_TYPE_ALREADY_PRESENT, symbol);
			return false;
		}

		// check 6
		if (this->hasSolid)
		{
			Log::e(STR_SOLID_PRESENT_CANT_ADD, symbol);
			return false;
		}

		// check 7
		if (this->hasStairs)
		{
			Log::e(STR_STAIRS_PRESENT_CANT_ADD, symbol);
			return false;
		}

		std::shared_ptr<sf::Texture> txt = resMgr.getTexture(mat->texturePath);
		if (txt != nullptr)
			txt->setRepeated(true);

		this->platform.setTexture(txt);
		this->platform.setTextureRect({
			static_cast<int>(this->getPosition().x),
			static_cast<int>(this->getPosition().y),
			CELL_SIDE_LEN,
			CELL_SIDE_LEN
		});

		this->hasPlatform = true;
	}
	else if (mat->type == MAT_STAIRS)
	{
		// check 4 (stairs)
		if (this->hasStairs)
		{
			Log::e(STR_MAT_SYMBOL_TYPE_ALREADY_PRESENT, symbol);
			return false;
		}

		// check 7
		if (this->hasPlatform)
		{
			Log::e(STR_PLATFORM_PRESENT_CANT_ADD, symbol);
			return false;
		}

		// check 8
		if (this->hasSolid)
		{
			Log::e(STR_SOLID_PRESENT_CANT_ADD, symbol);
			return false;
		}

		this->stairs.setTexture(resMgr.getTexture(mat->texturePath));
		this->stairs.setPosition({ static_cast<float>(mat->offsetLeft), 0 });

		this->hasStairs = true;
	}
	else if (mat->type == MAT_LIQUID)
	{
		// check 4 (liquid)
		if (this->hasLiquid)
		{
			Log::e(STR_MAT_SYMBOL_TYPE_ALREADY_PRESENT, symbol);
			return false;
		}

		this->liquidDelim.setTexture(resMgr.getTexture(mat->textureDelimPath));
		this->liquidDelim.setColor(liquidSpriteColor);

		this->topCellBlocksLiquidDelim = topCellBlocksLiquidDelim;
		this->liquid.setFillColor(mat->color);
		this->hasLiquid = true;
	}
	else
	{
		// check 3
		Log::e(STR_MAT_MISSING_OR_WRONG_TYPE, symbol);
		return false;
	}

	return true;
}

/**
 * Determines if the cell below this cell should draw a ladder delimeter (if it's a ladder)
 */
bool RoomCell::blocksBottomCellLadderDelim() const
{
	return this->hasLadder || this->hasSolid;
}

/**
 * Determines if the cell below this cell should draw a liquid delimeter (surface) (if it's a liquid)
 */
bool RoomCell::blocksBottomCellLiquidDelim() const
{
	return this->hasLiquid || this->hasSolid;
}

bool RoomCell::getHasSolid() const
{
	return this->hasSolid;
}

/**
 * @return true if the Cell is collider (any type)
 * @return false if the Cell is not a collider
 */
bool RoomCell::getIsCollider() const
{
	return this->isCollider;
}

const sf::FloatRect& RoomCell::getSolidCollider() const
{
	return this->solidCollider;
}

/**
 * @brief Finalizes Cell setup after all symbols have been added and performs sanity checks.
 *
 * Checks include:
 *   1. Liquid + solid without height flag is an invalid case
 *
 * @return true if the cell is sane
 * @return false if the cell contains errors
 */
bool RoomCell::finishSetup()
{
	if (this->hasLiquid && this->hasSolid && this->topOffset == 0)
	{
		Log::e(STR_LIQUID_SOLID_NO_HEIGHT_FLAG);
		return false;
	}

	// set the correct texture rect for solid. we also want to take part-height flag into account, by moving the sprite
	// down and decreasing height of the texture rect. we can't do it inside ::addSolidSymbol(), as the height flag (if
	// any) is not yet added at that point.
	this->solid.setPosition({ 0, static_cast<float>(this->topOffset) });
	this->solid.setTextureRect({
		static_cast<int>(this->getPosition().x),
		static_cast<int>(this->getPosition().y) + this->topOffset,
		CELL_SIDE_LEN,
		static_cast<int>(CELL_SIDE_LEN) - this->topOffset
	});

	// set the collider flag and calculate the collider (if applicable). the three cases are mutually exclusive,
	// which should have already been ensured in ::addOtherSymbol().
	// for now only solid collider is supported
	if (this->hasSolid)
	{
		this->isCollider = true;

		this->solidCollider.left = this->getPosition().x;
		this->solidCollider.top = this->getPosition().y + this->topOffset;

		this->solidCollider.width = CELL_SIDE_LEN;
		this->solidCollider.height = CELL_SIDE_LEN - this->topOffset;
	}
	//else if (this->hasStairs)
	//{
	//	this->isCollider = true;
	// TODO
	//}
	//else if (this->hasPlatform)
	//{
	//	this->isCollider = true;
	// TODO
	//}

	return true;
}

/*
 * Cell drawing could potentially be optimized.
 *
 * One way to do this might be to have the sprite in Room::draw(), set it to one of Cell's textures, draw it. Do it in a
 * loop, then use another loop to draw other texture (e.g. background) for each Cell. This could prevent setting the
 * same texture on Sprite multiple times. Note: since it's only setting a pointer (see
 * https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Sprite.php), this might not actually give us any performance
 * improvement. We could go a step further and e.g. try to group Cells using the same texture into bigger shapes, then
 * set a texture on that single shape and draw it. Or we could have a map translating texture -> list of cells using it,
 * to avoid even more texture swapping. But again - this might not actually give us any performance benefits,
 * additionally complicating the code. For now let's keep all drawing logic inside the Cell. It also avoids duplicated
 * code in drawing a single cell (::redrawCell()), not whole room.
 */

/**
 * First stage of drawing the cell. Draws background.
 */
void RoomCell::drawBackground(sf::RenderTarget& target) const
{
	sf::RenderStates states(this->getTransform());

	if (this->hasBackground)
		target.draw(this->background, states);
}

/**
 * Second stage of drawing the cell. Draws platform.
 *
 * Should be called after ::drawBackground() was already called for this cell.
 */
void RoomCell::drawPlatform(sf::RenderTarget& target) const
{
	sf::RenderStates states(this->getTransform());

	if (this->hasPlatform)
		target.draw(this->platform, states);
}

/**
 * Third stage of drawing the cell. Draws stairs.
 *
 * Should be called after ::drawBackground() and ::drawPlatform() were already called for all cells.
 *
 * Stairs have sprites with parts visible outside of cell area, therefore they need to drawn differently
 * than elements contained within cell area. That is, they don't use texture repeating.
 *
 * Stairs need to be drawn separately from previous stages, as the surrounding cells might draw over the parts of stairs
 * outside cell area, which is undesirable.
 */
void RoomCell::drawStairs(sf::RenderTarget& target) const
{
	sf::RenderStates states(this->getTransform());

	if (this->hasStairs)
		target.draw(this->stairs, states);
}

/**
 * Fourth stage of drawing the cell. Draws ladder.
 *
 * Should be called after ::drawBackground(), ::drawPlatform() and ::drawStairs() were already called for all cells.
 *
 * Same logic as in ::drawStairs() applies to ladders as well, i.e. ladders have parts that are sticking out of cell
 * area, so they need to be drawn separately.
 */
void RoomCell::drawLadder(sf::RenderTarget& target) const
{
	sf::RenderStates states(this->getTransform());

	if (this->hasPlatform || this->hasStairs || this->topCellBlocksLadderDelim)
		target.draw(this->ladder, states);
	else
		target.draw(this->ladderDelim, states);
}

/**
 * Fifth stage of drawing a cell. Draws solid and liquid.
 *
 * Solid Snake and Liquid Snake, what a coincidence.
 *
 * Should be called after all other draw methods were already called for all cells.
 *
 * Liquid and solid need to be drawn over ladders and stairs. In case of liquid, it will create the effect of submerging
 * stuff. In case of solid, it will prevent the parts of stairs/ladders that are sticking out of their cell from being
 * displayed over solids, which would not make sense.
 */
void RoomCell::drawLiquidAndSolid(sf::RenderTarget& target) const
{
	sf::RenderStates states(this->getTransform());

	if (this->hasLiquid)
	{
		if (this->topCellBlocksLiquidDelim)
			target.draw(this->liquid, states);
		else
			target.draw(this->liquidDelim, states);
	}

	if (this->hasSolid)
		target.draw(this->solid, states);
}
