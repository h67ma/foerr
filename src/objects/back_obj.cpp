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
								const struct back_obj_data &backObjData, enum LightObjectsState lightState) const
{
	int selectedVariant = backObjData.variantIdx;

	// explicitly set variant idx (>=0) overwrites everything else
	if (selectedVariant < 0)
	{
		// note: this method of switching light "on" or "off" is used mostly to keep all Remains rooms looking the same
		// as in original. the way it's done there is:
		// room.lon == 1 (lights on) -> frame = lon (always set to 1, which is first variant)
		// room.lon == -1 (lights off) -> frame = loff (set to first variant which doesn't have light texture)
		// alternative way to select on/off variant:
		// on: getRandomBetween(0, lightCnt - 1)
		// off: getRandomBetween(lightCnt, mainCnt)
		if (this->lightCnt > 0 && lightState == LIGHTS_ON)
			selectedVariant = 0;
		else if (this->lightCnt > 0 && lightState == LIGHTS_OFF)
			selectedVariant = this->lightCnt;
		else if (selectedVariant < 0)
			selectedVariant = Randomizer::getRandomBetween(0, this->variantsCnt - 1);
	}

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
