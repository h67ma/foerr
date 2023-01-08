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
								   ResourceManager &resMgr, const std::string &objId, int variantIdx) const
{
	auto search = this->objects.find(objId);
	if (search == this->objects.end())
		return false;

	search->second.setupBgSprites(mainSpriteRes, lightSpriteRes, resMgr, objId, variantIdx);
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
									   ResourceManager &resMgr, const std::string &objId, int variantIdx) const
{
	auto search = this->holeObjects.find(objId);
	if (search == this->holeObjects.end())
		return false;

	search->second.setupBgSprites(mainSpriteRes, holeSpriteRes, blend, resMgr, objId, variantIdx);
	return true;
}
