// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023-2024 h67ma <szycikm@gmail.com>

#include "back_hole_obj.hpp"

#include "../util/json.hpp"
#include "../util/random.hpp"

bool BackHoleObject::loadFromJson(const nlohmann::json& jsonNode)
{
	parseJsonKey<uint>(jsonNode, PATH_OBJS, FOERR_JSON_KEY_MAIN_CNT, this->variantsCnt);

	// we assume that the number of hole textures equals the number of main textures, therefore no need to store it
	// separately - just store both as ::variantsCnt

	parseJsonKey<bool>(jsonNode, PATH_OBJS, FOERR_JSON_KEY_BLEND, this->blend, true);

	parseJsonVector2Key<float>(jsonNode, PATH_OBJS, FOERR_JSON_KEY_OFFSET, this->offset, true);

	// note: hole objects don't support alpha attribute

	// the only condition for the object being valid is that there's at least one variant
	return this->variantsCnt > 0;
}

bool BackHoleObject::setupBgSprites(SpriteResource& mainSpriteRes, SpriteResource& holeSpriteRes, bool& blend,
									ResourceManager& resMgr, const struct back_obj_data& backObjData) const
{
	int selectedVariant = backObjData.variantIdx;

	if (selectedVariant < 0)
		selectedVariant = Randomizer::getRandomBetween(0, this->variantsCnt - 1);
	else if (selectedVariant >= this->variantsCnt)
		return false;

	mainSpriteRes.setTexture(resMgr.getTexture(litSprintf("%s/%s_%d%s", PATH_TEXT_OBJS_BACK.c_str(),
														  backObjData.id.c_str(), selectedVariant, TXT_MAIN_SUFFIX)));
	mainSpriteRes.setPosition(this->offset);
	mainSpriteRes.setColor(BACK_OBJ_COLOR);

	holeSpriteRes.setTexture(resMgr.getTexture(litSprintf("%s/%s_%d%s", PATH_TEXT_OBJS_BACK.c_str(),
														  backObjData.id.c_str(), selectedVariant, TXT_HOLE_SUFFIX)));
	holeSpriteRes.setPosition(this->offset);

	blend = this->blend;

	return true;
}
