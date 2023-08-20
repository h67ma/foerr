#include "back_hole_obj.hpp"

#include "../util/random.hpp"

bool BackHoleObject::loadFromJson(const json &jsonNode)
{
	parseJsonKey<uint>(jsonNode, std::string(PATH_OBJS), FOERR_JSON_KEY_MAIN_CNT, this->variantsCnt);

	// we assume that the number of hole textures equals the number of main textures, therefore no need to store it
	// separately - just store both as ::variantsCnt

	parseJsonKey<bool>(jsonNode, std::string(PATH_OBJS), FOERR_JSON_KEY_BLEND, this->blend, true);

	parseJsonVector2Key<float>(jsonNode, std::string(PATH_OBJS), FOERR_JSON_KEY_OFFSET, this->offset, true);

	// the only condition for the object being valid is that there's at least one variant
	return this->variantsCnt > 0;
}

bool BackHoleObject::setupBgSprites(SpriteResource &mainSpriteRes, SpriteResource &holeSpriteRes, bool &blend,
									ResourceManager &resMgr, const std::string &objId, int variantIdx) const
{
	if (variantIdx < 0)
		variantIdx = Randomizer::getRandomBetween(0, this->variantsCnt - 1);
	else if (variantIdx >= this->variantsCnt)
		return false;

	mainSpriteRes.setTexture(resMgr.getTexture(litSprintf("%s/%s_%d%s", PATH_TEXT_OBJS_BACK, objId.c_str(), variantIdx,
														  TXT_MAIN_SUFFIX)));
	mainSpriteRes.setPosition(this->offset);
	mainSpriteRes.setColor(BACK_OBJ_COLOR);

	holeSpriteRes.setTexture(resMgr.getTexture(litSprintf("%s/%s_%d%s", PATH_TEXT_OBJS_BACK, objId.c_str(), variantIdx,
														  TXT_HOLE_SUFFIX)));
	holeSpriteRes.setPosition(this->offset);

	blend = this->blend;

	return true;
}
