#pragma once

#include <string>

#include "../resources/resource_manager.hpp"
#include "../resources/sprite_resource.hpp"
#include "back_obj_base.hpp"
#include "../util/json.hpp"

/**
 * BackHoleObject is used for back objects which create a hole in background/backwall. This is achieved by defining two
 * textures: one normal ("main"), and one a mask, defining what parts of background should be removed ("hole"). The
 * cutout effect is achieved via proper blending mode when drawing a Room.
 *
 * Holes need to be kept in a separate collection than regular back objects, because of the desired order of drawing:
 *   - Background/backwall
 *   - Holes main textures
 *   - Making an actual holes in everything that's already drawn
 *   - Drawing the rest of back objects, so that they won't be trimmed by holes
 */
class BackHoleObject : public BackObjectBase
{
	private:
		// apart from "overlay" blend (used only for holes), Remains also uses "multiply" for some objects (blood1 and
		// blood2), but it doesn't seem to make sense, so let's ignore it for now and only have a flag for overlay
		// blending mode here
		bool blend = false;

	public:
		bool loadFromJson(const json &jsonNode);
		bool setupBgSprites(SpriteResource &mainSpriteRes, SpriteResource &holeSpriteRes, bool &blend,
						    ResourceManager &resMgr, const std::string &objId, int variantIdx) const;
};
