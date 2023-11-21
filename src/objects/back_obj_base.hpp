// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <SFML/System/Vector2.hpp>
#include <nlohmann/json.hpp>

#include "../util/util.hpp"

// TODO find out the exact shade
#define BACK_OBJ_COLOR COLOR_GRAY(120)
#define BACK_OBJ_COLOR_ALPHA(alpha) COLOR_GRAY_ALPHA(120, alpha)

#define TXT_MAIN_SUFFIX ".png"
#define TXT_HOLE_SUFFIX "_h.png"
#define TXT_LIGHT_SUFFIX "_l.png"

/**
 * A Back Object is a visual object that appears in Room background. It can appear either over backwall & background, or
 * behind them (depends in which collection in Room it is defined - "back_objs" or "far_back_objs"). Holes are handled
 * separately ("back_holes").
 *
 * The object does not store paths of textures; instead, because of the strict naming convention, it derives filenames
 * from object id (stored in BackObjectsManager as map key and passed to ::setupBgSprites()), texture variant, and type
 * (main/hole/light).
 *
 * A "variant" is an alternative texture for a given object. For example object "door" could have 4 textures, each
 * one a slightly different door. By displaying one of N variants for each object in a given Room, the environment gains
 * visual variety, because every time a Room is prepared, a different set of object textures will be picked.
 *
 * While it makes sense to have an object which has several possible textures, there might be cases when a specific
 * variant is needed (e.g. we really really want to display variant 2 of "door" object, because it just fits the scene).
 * For this purpose, objects placed in Rooms can overwrite picking a random variant by specifying "var" field with
 * variant index. Note: light objects can additionally be turned "on" or "off" - see BackObject.
 *
 * BackObject stores only one 2d offset per all types and all variants inside them. The idea is that the different
 * variants are representing the same general object, therefore their size should be the same. If one of them needs to
 * be slightly smaller, it can add padding to the image itself. Note: offset might be different for each type of
 * texture.
 *
 * Types of textures:
 *   - Main - the main object texture, displayed with reduced brightness (same as Room backwalls/background). This case
 *			  is handled by BackObject class.
 *   - Hole - a mask that defines area which should be "trimmed" from combined Room backwalls, background, and other
 *			  objects' main textures. An example of this is a window - main texture would be the frame, and hole would
 *			  be the parts with glass. This case is handled by BackHoleObject class.
 *   - Light - usually a bright radial gradient. Usually displayed over main texture of some sort of lamp. This case
 *			   is handled by BackObject class.
 */
class BackObjectBase
{
	protected:
		sf::Vector2f offset{ 0.F, 0.F };
		uint variantsCnt = 0;

	public:
		virtual bool loadFromJson(const nlohmann::json& jsonNode) = 0;
};
