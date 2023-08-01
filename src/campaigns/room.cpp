#include "room.hpp"

#include <string>
#include <memory>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "../settings/settings_manager.hpp"
#include "../objects/back_obj.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

#define ROOM_SYMBOL_SEPARATOR '|'
#define ROOM_SYMBOL_EMPTY '_'
#define ROOM_SYMBOL_UNKNOWN '?'

Room::Room(Player &player) : player(player)
{
	// "The box is there for a reason. I like thinking inside of it. I feel safe in there."
}

/**
 * Loads the room data and stores it in this object.
 *
 * Room node structure:
 * {
 *	"coords": [2, 0, 1],	// (X, Y, Z). Z=0 is the front/main layer, Z=1 is backstage-1, Z=2 is backstage-2, etc.
 *	"is_start": true,		// optional, exactly one room in a location must be a start room
 *	"backwall": "tName",	// optional, name of texture which should be displayed where no cell background is defined
 *	"cells": [
 *		"H|_-|...",
 *		"_Ka|G;|...",
 *		...
 *	]
 * }
 *
 * The cell data is separated by ROOM_SYMBOL_SEPARATOR ('_'). Each cell is of 1 or more characters in length.
 * First character in a cell corresponds to a solid, and can be empty ('_').
 * Rest of the symbols define various elements, such as background, stairs, ladder, water.
 * The dimensions of cell grid are always 48x25 cells (including border cells).
 *
 * To overwrite full background, just define a backwall for the room.
 *
 * Note: while it would be the most efficient (both in storage and in processing time) to store room data in binary
 * format, this approach has downsides. First, text is easily editable, which allows casual hackers to experiment with
 * modifying rooms using only a text editor. It also makes it much easier to develop and debug the game, or tools used
 * for creating levels. Secondly, it makes it possible to track changes in data using version control, which is useful
 * for keeping file history, blaming, etc. This applies to all data files, but rooms are the most obvious candidate for
 * binarization as they are the largest in volume.
 *
 * @param resMgr reference to Resource Manager
 * @param matMgr reference to Material Manager
 * @param objMgr reference to Object Manager
 * @param root reference to json node containing room data
 * @param filePath location file path, just for printing
 * @returns true on load success
 * @returns false on load fail
 */
bool Room::load(ResourceManager &resMgr, const MaterialManager &matMgr, const ObjectManager &objMgr,
				const json &root, const std::string &filePath)
{
	///// room-wide backwall /////

	// backwall can be empty
	std::string backwallTxtPath;
	parseJsonKey<std::string>(root, filePath, FOERR_JSON_KEY_BACKWALL, backwallTxtPath, true);
	if (!backwallTxtPath.empty())
	{
		backwallTxtPath = pathCombine(PATH_TEXT_CELLS, backwallTxtPath + ".png");
		std::shared_ptr<sf::Texture> backwallTxt = resMgr.getTexture(backwallTxtPath);
		backwallTxt->setRepeated(true);
		this->backwall.setTexture(backwallTxt);
		this->backwall.setTextureRect({ 0, 0, GAME_AREA_WIDTH, GAME_AREA_HEIGHT });
		this->backwall.setColor(BACKWALL_COLOR);
	}

	///// room-wide liquid level /////

	// if liquid level is defined and > 0, room is fully submerged to that level (counting from bottom, in cells).
	// solids are also submerged
	this->liquidLevelHeight = 0;
	parseJsonKey<uint>(root, filePath, FOERR_JSON_KEY_LIQUID_LEVEL, this->liquidLevelHeight, true);
	if (this->liquidLevelHeight > 0)
	{
		std::string liquidSymbolStr;
		if (!parseJsonKey<std::string>(root, filePath, FOERR_JSON_KEY_LIQUID_SYMBOL, liquidSymbolStr))
			return false;

		if (liquidSymbolStr.length() != 1)
		{
			Log::e(STR_MAT_LOAD_KEY_NOT_1CHAR, liquidSymbolStr.c_str());
			return false;
		}

		char liquidSymbol = liquidSymbolStr[0];

		const struct material *liquidMat = matMgr.getOther(liquidSymbol);
		if (liquidMat == nullptr || liquidMat->type != MAT_LIQUID)
		{
			Log::e(STR_MAT_MISSING_OR_WRONG_TYPE, liquidSymbol);
			return false;
		}

		uint liquidLevelPx = CELL_SIDE_LEN * this->liquidLevelHeight;
		this->liquid.setSize(sf::Vector2f(GAME_AREA_WIDTH, liquidLevelPx));
		this->liquid.setPosition(0, GAME_AREA_HEIGHT - liquidLevelPx);
		this->liquid.setFillColor(liquidMat->color);
		this->liquidDelim.setTexture(resMgr.getTexture(liquidMat->textureDelimPath));
		this->liquidDelim.setColor(RoomCell::liquidSpriteColor);
	}

	///// spawn coords /////

	// spawn coords are usually only defined for the first room
	parseJsonVector2Key<uint>(root, filePath, FOERR_JSON_KEY_SPAWN_COORDS, this->spawnCoords, true);

	///// cells /////

	auto cellsSearch = root.find(FOERR_JSON_KEY_CELLS);
	if (cellsSearch == root.end())
	{
		Log::e(STR_MISSING_KEY, filePath.c_str(), FOERR_JSON_KEY_CELLS);
		return false;
	}

	if (!cellsSearch->is_array())
	{
		Log::e(STR_INVALID_TYPE, filePath.c_str(), FOERR_JSON_KEY_CELLS);
		return false;
	}

	if (cellsSearch->size() != ROOM_HEIGHT_WITH_BORDER)
	{
		Log::e(STR_INVALID_ARR_SIZE, filePath.c_str(), FOERR_JSON_KEY_CELLS, ROOM_HEIGHT_WITH_BORDER, cellsSearch->size());
		return false;
	}

	for (auto it = cellsSearch->begin(); it != cellsSearch->end(); it++)
	{
		std::string line;

		try
		{
			line = (*it);
		}
		catch (const json::type_error &ex)
		{
			Log::e(STR_INVALID_TYPE_EX, filePath.c_str(), FOERR_JSON_KEY_CELLS, ex.what());
			return false;
		}

		uint y = static_cast<uint>(std::distance(cellsSearch->begin(), it));
		uint x = 0;

		bool firstCellSymbol = true;
		for (const auto symbol : line)
		{
			if (symbol == ROOM_SYMBOL_SEPARATOR)
			{
				x++;
				firstCellSymbol = true;
				continue;
			}

			if (x >= ROOM_WIDTH_WITH_BORDER)
			{
				Log::w(STR_ROOM_LINE_TOO_LONG, filePath.c_str(), FOERR_JSON_KEY_CELLS, ROOM_WIDTH_WITH_BORDER);
				break;
			}

			if (firstCellSymbol)
			{
				// first symbol (solid)
				firstCellSymbol = false;

				// the cell needs to know its position to properly set texture rect, and to draw itself in correct place
				this->cells[y][x].setPosition(x * CELL_SIDE_LEN, y * CELL_SIDE_LEN);

				if (symbol == ROOM_SYMBOL_UNKNOWN)
				{
					Log::w(STR_UNKNOWN_SYMBOL_AT_POS, filePath.c_str(), FOERR_JSON_KEY_CELLS, x, y);
				}
				else if (symbol != ROOM_SYMBOL_EMPTY)
				{
					if (!this->cells[y][x].addSolidSymbol(symbol, resMgr, matMgr))
						return false;
				}
			}
			else if (symbol != ROOM_SYMBOL_EMPTY)
			{
				// 2nd, 3rd, etc. symbol, not empty

				// because we load cells Frgt/10 style (from the top to the bottom), we already know if a particular
				// cell needs to draw ladder or liquid delim based on the cell above it.
				// if y = 0 we don't want to draw delim
				bool topBlocksLadderDelim = y == 0 || this->cells[y - 1][x].blocksBottomCellLadderDelim();
				bool topBlocksLiquidDelim = y == 0 || this->cells[y - 1][x].blocksBottomCellLiquidDelim();

				if (symbol == ROOM_SYMBOL_UNKNOWN)
					Log::w(STR_UNKNOWN_SYMBOL_AT_POS, filePath.c_str(), FOERR_JSON_KEY_CELLS, x, y);
				else if (!this->cells[y][x].addOtherSymbol(symbol, topBlocksLadderDelim, topBlocksLiquidDelim, resMgr,
														   matMgr))
					return false;
			}
		}

		if (x != ROOM_WIDTH_WITH_BORDER - 1)
		{
			Log::e(STR_ROOM_ROW_TOO_SHORT, filePath.c_str(), FOERR_JSON_KEY_CELLS, y);
			return false;
		}

		for (auto &cell : this->cells[y])
		{
			if (!cell.finishSetup())
				return false;
		}
	}

	///// background objects /////

	if (!Room::parseBackObjsNode(root, filePath, resMgr, objMgr, FOERR_JSON_KEY_BACK_OBJS, false))
		return false;

	if (!Room::parseBackObjsNode(root, filePath, resMgr, objMgr, FOERR_JSON_KEY_FAR_BACK_OBJS, true))
		return false;

	if (!Room::parseBackHoleObjsNode(root, filePath, resMgr, objMgr))
		return false;

	return true;
}

bool Room::parseBackObjsNode(const json &root, const std::string &filePath, ResourceManager &resMgr,
							 const ObjectManager &objMgr, const char* key, bool far)
{
	// the room doesn't have to define any background objects (this is not an error)
	auto bgObjsSearch = root.find(key);
	if (bgObjsSearch != root.end())
	{
		if (!bgObjsSearch->is_array())
		{
			Log::e(STR_INVALID_TYPE, filePath.c_str(), key);
			return false;
		}

		for (const auto &backObjNode : *bgObjsSearch)
		{
			sf::Vector2u objCoords;
			if (!parseJsonVector2Key<uint>(backObjNode, filePath, FOERR_JSON_KEY_COORDS, objCoords))
				return false;

			std::string objId;
			if (!parseJsonKey<std::string>(backObjNode, filePath, FOERR_JSON_KEY_ID, objId))
				return false;

			int variantIdx;
			if (!parseJsonKey<int>(backObjNode, filePath, FOERR_JSON_KEY_VARIANT, variantIdx, true))
				variantIdx = -1;

			objCoords *= CELL_SIDE_LEN;

			SpriteResource backObjMain;
			SpriteResource backObjLight;
			if (!objMgr.setupBgSprites(backObjMain, backObjLight, resMgr, objId, variantIdx))
				Log::w(STR_BACK_OBJ_DEF_NOT_FOUND, objId.c_str());

			// note: move instead of setPosition, as objects were already moved according to offset
			backObjMain.move(static_cast<sf::Vector2f>(objCoords));

			if (far)
			{
				this->farBackObjectsMain.push_back(backObjMain);
				// light is unsupported for far back objects
			}
			else
			{
				//backObjLight.move(static_cast<sf::Vector2f>(objCoords)); // TODO
				this->backObjectsMain.push_back(backObjMain);
				//this->backObjects.push_back(backObjLight); // TODO
			}
		}
	}

	return true;
}

bool Room::parseBackHoleObjsNode(const json &root, const std::string &filePath, ResourceManager &resMgr,
								 const ObjectManager &objMgr)
{
	// the room doesn't have to define any hole objects (this is not an error)
	auto bgHolesSearch = root.find(FOERR_JSON_KEY_BACK_HOLES);
	if (bgHolesSearch != root.end())
	{
		if (!bgHolesSearch->is_array())
		{
			Log::e(STR_INVALID_TYPE, filePath.c_str(), FOERR_JSON_KEY_BACK_HOLES);
			return false;
		}

		for (const auto &backHoleNode : *bgHolesSearch)
		{
			sf::Vector2u objCoords;
			if (!parseJsonVector2Key<uint>(backHoleNode, filePath, FOERR_JSON_KEY_COORDS, objCoords))
				return false;

			std::string objId;
			if (!parseJsonKey<std::string>(backHoleNode, filePath, FOERR_JSON_KEY_ID, objId))
				return false;

			int variantIdx;
			if (!parseJsonKey<int>(backHoleNode, filePath, FOERR_JSON_KEY_VARIANT, variantIdx, true))
				variantIdx = -1;

			objCoords *= CELL_SIDE_LEN;

			SpriteResource backObjMain;
			SpriteResource backObjHole;
			bool blend;
			if (!objMgr.setupBgHoleSprites(backObjMain, backObjHole, blend, resMgr, objId, variantIdx))
				Log::w(STR_BACK_OBJ_DEF_NOT_FOUND, objId.c_str());

			// note: move instead of setPosition, as objects were already moved according to offset
			backObjMain.move(static_cast<sf::Vector2f>(objCoords));
			backObjHole.move(static_cast<sf::Vector2f>(objCoords));

			this->backHoleObjectsMain.push_back({
				.spriteRes = backObjMain,
				.blend = blend
			});
			this->backHoleObjectsHoles.push_back(backObjHole);
		}
	}

	return true;
}

/**
 * Prepares the Room to be drawn.
 * Should be called *only once* per entering the Room. After that, use ::redrawCell() to update cells.
 */
void Room::init()
{
	// TODO? calling the same nested loop multiple times is pretty lame, maybe find some better way to handle this.
	// one possible improvement might be to draw on all three textures (i.e. back, front1, front2) simultaneously.
	// this way we would only need two nested for loops. however, this approach would require having three
	// RenderTextures, and would make the code much harder to understand, so let's skip it for now.

	sf::RenderStates states;
	sf::RenderTexture tmpRender;
	tmpRender.create(GAME_AREA_WIDTH, GAME_AREA_HEIGHT);

	///// background cache /////

	tmpRender.clear(sf::Color::Transparent);

	// we could draw far back objects on another texture, along with background full, so that far back objects won't
	// move during room transition. the current approach looks visually ok though, so let's keep it. as a bonus we don't
	// have to add another caching texture.

	tmpRender.draw(this->backwall); // can be empty

	// note: in Remains far back object drawing seems to work a bit differently: they seem to be drawn over cell
	// backgrounds, but it makes little sense. in that case just use a regular, non-far back object. because of this,
	// some rooms might look a bit different than in Remains. for an example compare M.A.S. (2, 8, 0).
	for (const auto &backObj : this->farBackObjectsMain)
	{
		// blend mode not supported for far back objects
		tmpRender.draw(backObj);
	}

	for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			this->cells[y][x].drawBackground(tmpRender);
		}
	}

	for (const auto &backObj : this->backHoleObjectsMain)
	{
		if (backObj.blend)
			states.blendMode = BlendOverlayOrSomething;
		else
			states.blendMode = sf::BlendAlpha;

		tmpRender.draw(backObj.spriteRes, states);
	}

	states.blendMode = BlendSubtractOrSomething;
	for (const auto &backObj : this->backHoleObjectsHoles)
	{
		tmpRender.draw(backObj, states);
	}

	for (const auto &backObj : this->backObjectsMain)
	{
		tmpRender.draw(backObj);
	}

	tmpRender.display();

	// sf::RenderTexture can't be a private member because it inherits NonCopyable.
	// because of this we need to store the result in a standard sf::Texture
	this->backCacheTxt = tmpRender.getTexture();
	this->backCache.setTexture(this->backCacheTxt);

	///// front cache 1 - behind the Player /////

	tmpRender.clear(sf::Color::Transparent);

	for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			this->cells[y][x].drawPlatform(tmpRender);
		}
	}

	for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			this->cells[y][x].drawStairs(tmpRender);
		}
	}

	tmpRender.display();

	// again, can't keep sf::RenderTexture as a member (see above)
	this->frontCache1Txt = tmpRender.getTexture();
	this->frontCache1.setTexture(this->frontCache1Txt);

	///// front cache 2 - before the Player /////

	tmpRender.clear(sf::Color::Transparent);

	for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			this->cells[y][x].drawLadder(tmpRender);
		}
	}

	for (uint y = 0; y < ROOM_HEIGHT_WITH_BORDER; y++)
	{
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			this->cells[y][x].drawLiquidAndSolid(tmpRender);
		}
	}

	if (SettingsManager::debugBoundingBoxes)
	{
		// we draw this debug overlay on front cache, because we don't want it covered with front cache elements.
		// the overlay can still be covered by water level, but it's transparent. and we can't draw on water level
		// cache, as it is not always displayed.
		sf::RectangleShape debugBox;
		debugBox.setFillColor(sf::Color::Transparent);
		debugBox.setOutlineThickness(1.F);

		debugBox.setOutlineColor(sf::Color::Green);
		for (const auto &backObj : this->farBackObjectsMain)
		{
			debugBox.setPosition(backObj.getPosition());
			debugBox.setSize({ backObj.getLocalBounds().width, backObj.getLocalBounds().height });
			tmpRender.draw(debugBox);
		}

		debugBox.setOutlineColor(sf::Color::Red);
		for (const auto &backObj : this->backHoleObjectsMain)
		{
			debugBox.setPosition(backObj.spriteRes.getPosition());
			debugBox.setSize({ backObj.spriteRes.getLocalBounds().width, backObj.spriteRes.getLocalBounds().height });
			tmpRender.draw(debugBox);
		}

		debugBox.setOutlineColor(sf::Color::Cyan);
		for (const auto &backObj : this->backObjectsMain)
		{
			debugBox.setPosition(backObj.getPosition());
			debugBox.setSize({ backObj.getLocalBounds().width, backObj.getLocalBounds().height });
			tmpRender.draw(debugBox);
		}
	}

	tmpRender.display();

	// again, can't keep sf::RenderTexture as a member (see above)
	this->frontCache2Txt = tmpRender.getTexture();
	this->frontCache2.setTexture(this->frontCache2Txt);

	///// room-wide liquid level /////

	// we also need to pre-render liquid level. because of transparency and a sprite used for surface, the alpha will
	// get messed up if we simply draw it on top of liquid level rectangle. to counter this, we use sf::BlendNone.
	// but it would be difficult to use it along other elements (cells, backwall, etc.), therefore RenderTexture.
	tmpRender.clear(sf::Color::Transparent);
	states.blendMode = sf::BlendNone;

	// liquid level rectangle
	tmpRender.draw(this->liquid, states);

	// delims (surface)
	if (this->liquidLevelHeight > 0 && this->liquidLevelHeight < ROOM_HEIGHT_WITH_BORDER)
	{
		// we only need to check the row above room-wide water level
		uint y = ROOM_HEIGHT_WITH_BORDER - this->liquidLevelHeight;
		for (uint x = 0; x < ROOM_WIDTH_WITH_BORDER; x++)
		{
			if (!this->cells[y - 1][x].blocksBottomCellLiquidDelim() && !this->cells[y][x].getHasSolid())
			{
				this->liquidDelim.setPosition(x * CELL_SIDE_LEN, y * CELL_SIDE_LEN);
				tmpRender.draw(this->liquidDelim, states);
			}
		}
	}

	// again, can't keep sf::RenderTexture as a member (see above)
	this->cachedLiquidLevelTxt = tmpRender.getTexture();
	this->cachedLiquidLevel.setTexture(this->cachedLiquidLevelTxt);
}

/**
 * Clears cached textures used for rendering the Room.
 * Should be called when the Room is no longer displayed.
 */
void Room::deinit()
{
	this->backCacheTxt = sf::Texture();
	this->frontCache1Txt = sf::Texture();
	this->frontCache2Txt = sf::Texture();
	this->cachedLiquidLevelTxt = sf::Texture();
}

/**
 * Calculates new velocities of every movable object inside the Room based on previous object velocities and gravity.
 * Detects (AABB) and resolves collisions.
 */
void Room::tick(uint lastFrameDurationUs)
{
	// calculate object velocity in the middle of the frame
	this->player.updateVelocity(lastFrameDurationUs);

	// TODO with this simple collision detection method, there's a lower limit on FPS the game should run on in order
	// for collisions to work properly. if FPS is too low, frame times will be high, resulting in objects traveling
	// larger distances on every tick, potentially overshooting objects they should collide with in normal
	// circumstances.
	// assuming a tickrate of 30 FPS, tick duration is around 33333 microseconds.
	// assuming the player is 60x60 and cells are 40x40, the maximum velocity the player can have in order for the
	// collision with cell to be detected should be around 0.003. for comparison, the base sprint velocity is currently
	// 0.0008, around 2.6 times smaller, but comparable.
	// for now the current approach will do, but the game will surely have objects that move faster than that, and could
	// also potentially be played on low-end hardware, or suffer from lag spikes, so another collision detection method
	// should be developed, one that takes into account any colliders that are between the new and old object position.

	// TODO potential optimization: before checking & resolving collisions, clusterize cells into bigger rectangles.
	// put the bigger shapes in a vector and iterate through it (it will also eliminate the need to repeat that awkward
	// nested for loop). the clusterization algo can generate suboptimal results as long as it's fast.


	// object position is counted from center - subtract its halfed size to get top left corner
	float playerX = this->player.getPosition().x - PLAYER_W2;
	float playerY = this->player.getPosition().y - PLAYER_H2;

	// coordinates of the cell the object center resides in
	int playerCellX = static_cast<int>(playerX / CELL_SIDE_LEN);
	int playerCellY = static_cast<int>(playerY / CELL_SIDE_LEN);

	sf::FloatRect objCollider(playerX, playerY, PLAYER_W, PLAYER_H);

	// move the object on X axis, resolve X collision, then move the object on Y axis, resolve Y collision. based on:
	// https://gamedev.stackexchange.com/questions/69339/2d-aabbs-and-resolving-multiple-collisions/71123#71123

	float velocity = this->player.getVelocity().x;
	bool stop = false;

	if (velocity != 0)
	{
		// move the object along X axis
		objCollider.left += velocity * lastFrameDurationUs;

		// only check the cells that are near the object
		for (uint y = std::max(0, playerCellY - 1); !stop && y < std::min(playerCellY + PLAYER_CELL_H + 1, ROOM_HEIGHT_WITH_BORDER); y++)
		{
			for (uint x = std::max(0, playerCellX - 1); x < std::min(playerCellX + PLAYER_CELL_W + 1, ROOM_WIDTH_WITH_BORDER); x++)
			{
				const RoomCell& cell = this->cells[y][x];
				if (!cell.getIsCollider())
					continue;

				const sf::FloatRect& cellCollider = cell.getSolidCollider(); // TODO other types of colliders (stairs, platform)
				sf::FloatRect intersection;
				if (!objCollider.intersects(cellCollider, intersection))
					continue;

				float deltaX = intersection.width;
				if (velocity > 0)
					deltaX = -deltaX;

				objCollider.left += deltaX;
				player.stopHorizontal();

				stop = true;
				break;
			}
		}
	}

	velocity = this->player.getVelocity().y;
	stop = false;

	if (velocity != 0)
	{
		// move the object along Y axis
		objCollider.top += velocity * lastFrameDurationUs;

		// only check the cells that are near the object
		for (uint y = std::max(0, playerCellY - 1); !stop && y < std::min(playerCellY + PLAYER_CELL_H + 1, ROOM_HEIGHT_WITH_BORDER); y++)
		{
			for (uint x = std::max(0, playerCellX - 1); x < std::min(playerCellX + PLAYER_CELL_W + 1, ROOM_WIDTH_WITH_BORDER); x++)
			{
				const RoomCell& cell = this->cells[y][x];
				if (!cell.getIsCollider())
					continue;

				const sf::FloatRect& cellCollider = cell.getSolidCollider(); // TODO other types of colliders (stairs, platform)
				sf::FloatRect intersection;
				if (!objCollider.intersects(cellCollider, intersection))
					continue;

				float deltaY = intersection.height;
				if (velocity > 0)
					deltaY = -deltaY;

				objCollider.top += deltaY;
				player.stopVertical();

				stop = true;
				break;
			}
		}
	}

	// TODO? if jitter appears after resolving collisions, coords could be rounded
	this->player.setPosition(objCollider.left + PLAYER_W2, objCollider.top + PLAYER_H2);

	// TODO also check collisions with other objects (doors, bullets, characters, etc.)

	// remaining velocity update
	this->player.updateVelocity(lastFrameDurationUs);
}

/**
 * Returns spawn coordinates, in cell units.
 *
 * Note: if the Room does not have spawn coordinates defined, center of the Room will be returned.
 */
sf::Vector2u Room::getSpawnCoords() const
{
	return this->spawnCoords;
}

/**
 * Checks if a Cell at given coordinates is a collider (has solid).
 * Assumes that the Room has already been loaded via ::load().
 * If the coordinates are out of range, true is returned and a warning is logged.
 *
 * @param x Cell x coordinate
 * @param y Cell y coordinate
 * @return true if the Cell is a collider, or its coordinates are out of range
 * @return false if the Cell is not a collider
 */
bool Room::isCellCollider(uint x, uint y) const
{
	if (x >= ROOM_WIDTH_WITH_BORDER || y >= ROOM_HEIGHT_WITH_BORDER)
	{
		Log::w(STR_IDX_OUTTA_BOUNDS);
		return true;
	}

	return this->cells[y][x].getHasSolid();
}

/**
 * @brief Redraws front elements of a single Cell. For drawing the entire Room, use ::draw()
 *
 * We'll never want to redraw Cell background, as it will never change.
 *
 * If cell coordinates are invalid, nothing will happen.
 *
 * @param x cell x coordinate
 * @param y cell y coordinate
 * @param target reference to render target
 * @param states render states
 */
void Room::redrawCell(uint x, uint y, sf::RenderTarget &target, sf::RenderStates states) const
{
	if (x >= ROOM_WIDTH_WITH_BORDER || y >= ROOM_HEIGHT_WITH_BORDER)
		return;

	const RoomCell *cell = &this->cells[y][x];
	cell->drawPlatform(target);
	cell->drawStairs(target);
	cell->drawLadder(target);
	cell->drawLiquidAndSolid(target);
}

void Room::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	// because we render stuff first to RenderTexture, and then the texture to target, alpha gets blended two times,
	// and therefore gets screwed up. to fix it, use a custom blending mode.
	states.blendMode = BlendAlphaTransparent;
	target.draw(this->backCache, states);
	target.draw(this->frontCache1, states);

	// player was not rendered to a texture, so use the default blending mode
	states.blendMode = sf::BlendAlpha;
	target.draw(this->player, states);

	states.blendMode = BlendAlphaTransparent;
	target.draw(this->frontCache2, states);

	// liquid is drawn over all cell elements, including solids
	// liquids were also rendered to RenderTexture, but using sf::BlendNone, therefore the default blending mode works
	// correctly here.
	if (this->liquidLevelHeight > 0)
	{
		states.blendMode = sf::BlendAlpha;
		target.draw(this->cachedLiquidLevel, states);
	}
}
