#include "room_cell.hpp"
#include <memory>
#include <SFML/Graphics/Sprite.hpp>
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

/**
 * @brief Adds a solid symbol to the cell
 * It is expected that the solid will be added to the cell before any other symbols.
 *
 * Performs checks before adding the symbol to ensure that the cell will be sane. This includes:
 *   1. No solid has already been added to the cell
 *   2. Material exists and its type is solid
 *
 * @param symbol symbol character
 * @return true if the symbol was added successfully
 * @return false if the symbol cannot be added
 */
bool RoomCell::addSolidSymbol(char symbol, ResourceManager &resMgr, const MaterialManager &matMgr)
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

	this->solidTxt.set(resMgr.getTexture(mat->texturePath));

	// TODO mask will probably be handled elsewhere
	if (mat->maskTexturePath != "")
		this->solidTxtMask.set(resMgr.getTexture(mat->maskTexturePath));

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
 *     - Stairs
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
 * @brief Performs sanity checks after all symbols have already been added. This includes:
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

/**
 * This could potentially be optimized.
 *
 * One way to do this might be to have the sprite in Room::draw(), set it to one of Cell's textures, draw it. Do it in a
 * loop, then use another loop to draw other texture (e.g. background) for each Cell. This could prevent setting the
 * same texture on Sprite multiple times. Note: since it's only setting a pointer (see
 * https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Sprite.php), this might not actually give us any performance
 * improvement. We could go a step further and e.g. try to group Cells using the same texture into bigger shapes, then
 * set a texture on that single shape and draw it. Or we could have a map translating texture -> list of cells using it,
 * to avoid even more texture swapping. But again - this might not actually give us any performance benefits,
 * additionally complicating the code. For now let's keep all drawing logic inside the Cell. It also avoids duplicated
 * code in drawing a single cell (::drawCell()), not whole room.
 */
void RoomCell::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	std::shared_ptr<sf::Texture> txt;

	sf::Sprite cellSprite;
	cellSprite.setTextureRect({
		static_cast<int>(this->getPosition().x),
		static_cast<int>(this->getPosition().y),
		CELL_SIDE_LEN,
		CELL_SIDE_LEN
	});

	// TODO background, others

	txt = this->solidTxt.get();
	if (txt != nullptr)
	{
		cellSprite.setTexture(*txt);
		target.draw(cellSprite, states);
	}
}
