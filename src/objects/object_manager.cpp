#include "object_manager.hpp"

#include "../util/json.hpp"
#include "../hud/log.hpp"

bool ObjectManager::load()
{
	json root;
	if (!loadJsonFromFile(root, PATH_OBJS))
		return false;

	auto bgObjsSearch = root.find(FOERR_JSON_KEY_BACK_OBJS);
	if (bgObjsSearch == root.end())
	{
		Log::e(STR_MISSING_KEY, PATH_OBJS, FOERR_JSON_KEY_BACK_OBJS);
		return false;
	}

	if (!bgObjsSearch->is_object())
	{
		Log::e(STR_INVALID_TYPE, PATH_OBJS, FOERR_JSON_KEY_BACK_OBJS);
		return false;
	}

	for (const auto &objNode : bgObjsSearch->items())
	{
		this->objects.emplace(objNode.key(), BackObject());
		if (!this->objects.at(objNode.key()).loadFromJson(objNode.value()))
		{
			this->objects.clear();
			return false;
		}
	}

	auto bgHoleObjsSearch = root.find(FOERR_JSON_KEY_BACK_HOLES);
	if (bgHoleObjsSearch == root.end())
	{
		Log::e(STR_MISSING_KEY, PATH_OBJS, FOERR_JSON_KEY_BACK_HOLES);
		return false;
	}

	if (!bgHoleObjsSearch->is_object())
	{
		Log::e(STR_INVALID_TYPE, PATH_OBJS, FOERR_JSON_KEY_BACK_HOLES);
		return false;
	}

	for (const auto &objNode : bgHoleObjsSearch->items())
	{
		this->holeObjects.emplace(objNode.key(), BackHoleObject());
		if (!this->holeObjects.at(objNode.key()).loadFromJson(objNode.value()))
		{
			this->holeObjects.clear();
			this->objects.clear();
			return false;
		}
	}

	return true;
}

/**
 * @brief Sets up sprite resources for the specified object id and variant
 *
 * @param mainSpriteRes reference to a sprite resource to use as main texture
 * @param lightSpriteRes reference to a sprite resource to use as light
 * @param resMgr reference to Resource Manager
 * @param objId object id/name (without extension and variant index)
 * @param variantIdx variant index to use, or negative to use random
 * @return true if setup was successful
 * @return false if setup has failed
 */
bool ObjectManager::setupBgSprites(SpriteResource &mainSpriteRes, SpriteResource &lightSpriteRes,
								   ResourceManager &resMgr, const struct back_obj_data &backObjData) const
{
	auto search = this->objects.find(backObjData.id);
	if (search == this->objects.end())
	{
		Log::e(STR_BACK_OBJ_SETUP_FAIL_DEF_MISSING, backObjData.id.c_str());
		return false;
	}

	if (!search->second.setupBgSprites(mainSpriteRes, lightSpriteRes, resMgr, backObjData))
		return false;

	// note: move() instead of setPosition(), as objects were already moved according to offset
	mainSpriteRes.move(static_cast<sf::Vector2f>(backObjData.coordinates));
	lightSpriteRes.move(static_cast<sf::Vector2f>(backObjData.coordinates));

	return true;
}

/**
 * @brief Sets up sprite resources for the specified hole object id and variant
 *
 * @param mainSpriteRes reference to a sprite resource to use as main texture
 * @param holeSpriteRes reference to a sprite resource to use as light
 * @param blend reference to blend variable
 * @param resMgr reference to Resource Manager
 * @param objId object id/name (without extension and variant index)
 * @param variantIdx variant index to use, or negative to use random
 * @return true if setup was successful
 * @return false if setup has failed
 */
bool ObjectManager::setupBgHoleSprites(SpriteResource &mainSpriteRes, SpriteResource &holeSpriteRes, bool &blend,
									   ResourceManager &resMgr, const struct back_obj_data &backObjData) const
{
	auto search = this->holeObjects.find(backObjData.id);
	if (search == this->holeObjects.end())
	{
		Log::e(STR_BACK_OBJ_SETUP_FAIL_DEF_MISSING, backObjData.id.c_str());
		return false;
	}

	if (!search->second.setupBgSprites(mainSpriteRes, holeSpriteRes, blend, resMgr, backObjData))
		return false;

	// note: move() instead of setPosition(), as objects were already moved according to offset
	mainSpriteRes.move(static_cast<sf::Vector2f>(backObjData.coordinates));
	holeSpriteRes.move(static_cast<sf::Vector2f>(backObjData.coordinates));

	return true;
}
