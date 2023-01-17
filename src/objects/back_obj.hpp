#pragma once

#include <string>

#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"
#include "back_obj_base.hpp"
#include "../util/json.hpp"

/**
 * BackObject is the "normal" back object. It can define a main texture and a light texture. Holes are handled by
 * BackHoleObject class.
 *
 * BackObjects can be displayed either normally (on top of backwall/background), or behind background.
 *
 * There are no restrictions for how many of each texture type should be defined (other than both being zero).
 * Basically, if at least one type is defined for the requested (or randomized) variant index, it is valid. When setting
 * up the two sprites, some of them could have their textures set to nullptr.
 */
class BackObject : public BackObjectBase
{
	private:
		uint mainCnt = 0;
		uint lightCnt = 0;

	public:
		bool loadFromJson(const json &jsonNode);
		bool setupBgSprites(SpriteResource &mainSpriteRes, SpriteResource &lightSpriteRes, ResourceManager &resMgr,
							const std::string &objId, int variantIdx) const;
};
