#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"
#include "light_objects_state.hpp"
#include "back_obj_base.hpp"
#include "back_obj_data.hpp"
#include "../consts.hpp"

/**
 * BackObject can define a main texture and a light texture. Both textures are optional, but obviously there needs to be
 * at least one texture of some type.
 *
 * There are no restrictions for how many of each texture type should be defined (other than both being zero).
 * Basically, if at least one type is defined for the requested (or randomized) variant index, it is valid. When setting
 * up the two sprites, some of them could have their textures set to nullptr. Possible cases:
 *	1. N main textures, 0 light textures - all variants will have a main texture, and nullptr light texture
 *	2. 0 main textures, M light textures - all variants will have nullptr main texture, and have a light texture
 *	3. N main textures, M light textures, N = M - all variants will have a main texture and a light texture
 *	4. N main textures, M light textures, N > M - first M variants are considered having "light on", rest are considered
 *		to be "light off". Depending on variant, light texture might be set or not. In this case the caller can request
 *		a random variant with light on, or with light off. Why not just make the same number of main and light textures?
 *		Well, so that we can display a nice main sprite of a turned off lamp, of course.
 *	5. N main textures, M light textures, N < M - this makes little sense, but is allowed and treated the same as case
 *		2, except that a few first variants will have main texture.
 *
 * BackObjects can be displayed either normally (on top of backwall/background), or behind background.
 */
class BackObject : public BackObjectBase
{
	private:
		sf::Vector2f offsetLight{ 0.F, 0.F };
		uint mainCnt = 0;
		uint lightCnt = 0;
		uchar alphaChannel = COLOR_MAX_CHANNEL_VALUE;

	public:
		bool loadFromJson(const nlohmann::json &jsonNode) override;
		bool setupBgSprites(SpriteResource &mainSpriteRes, SpriteResource &lightSpriteRes, ResourceManager &resMgr,
							const struct back_obj_data &backObjData, enum LightObjectsState lightState) const;
};
