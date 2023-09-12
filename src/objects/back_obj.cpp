#include "back_obj.hpp"

#include <algorithm>

#include "../util/random.hpp"

bool BackObject::loadFromJson(const json &jsonNode)
{
	// each can be undefined, in which case we assume 0
	parseJsonKey<uint>(jsonNode, std::string(PATH_OBJS), FOERR_JSON_KEY_MAIN_CNT, this->mainCnt, true);
	parseJsonKey<uint>(jsonNode, std::string(PATH_OBJS), FOERR_JSON_KEY_LIGHT_CNT, this->lightCnt, true);

	parseJsonVector2Key<float>(jsonNode, std::string(PATH_OBJS), FOERR_JSON_KEY_OFFSET, this->offset, true);
	parseJsonVector2Key<float>(jsonNode, std::string(PATH_OBJS), FOERR_JSON_KEY_OFFSET_LIGHT, this->offsetLight, true);

	this->variantsCnt = std::max(this->mainCnt, this->lightCnt);

	// the only condition for the object being valid is that at least one of the types defines at least one variant
	return this->variantsCnt > 0;
}

/**
 * @brief Sets up back object sprites with a variant of textures
 *
 * @param mainSpriteRes reference to a sprite resource to use as main texture
 * @param lightSpriteRes reference to a sprite resource to use as light
 * @param resMgr reference to Resource Manager
 * @param objId object id/name (without extension and variant index)
 * @param variantIdx zero-based variant index to use, or negative to use random
 * @return true if at least one of the types has set a texture
 * @return false if no types provided texture for requested variant
 */
bool BackObject::setupBgSprites(SpriteResource &mainSpriteRes, SpriteResource &lightSpriteRes, ResourceManager &resMgr,
								const struct back_obj_data &backObjData) const
{
	int selectedVariant = backObjData.variantIdx;

	if (selectedVariant < 0)
		selectedVariant = Randomizer::getRandomBetween(0, this->variantsCnt - 1);

	bool gotOne = false;

	if (selectedVariant < this->mainCnt)
	{
		mainSpriteRes.setTexture(resMgr.getTexture(litSprintf("%s/%s_%d%s", PATH_TEXT_OBJS_BACK,
															  backObjData.id.c_str(), selectedVariant,
															  TXT_MAIN_SUFFIX)));
		mainSpriteRes.setPosition(this->offset);

		// objects which are light sources are not dimmed
		if (this->lightCnt == 0)
			mainSpriteRes.setColor(BACK_OBJ_COLOR);

		gotOne = true;
	}

	// TODO support light on/off variants
	if (selectedVariant < this->lightCnt)
	{
		lightSpriteRes.setTexture(resMgr.getTexture(litSprintf("%s/%s_%d%s", PATH_TEXT_OBJS_BACK,
															   backObjData.id.c_str(), selectedVariant,
															   TXT_LIGHT_SUFFIX)));
		lightSpriteRes.setPosition(this->offsetLight);
		gotOne = true;
	}

	return gotOne;
}
