// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <unordered_map>
#include <string>

#include "back_hole_obj.hpp"
#include "back_obj_data.hpp"
#include "back_obj.hpp"

/**
 * ObjectManager is used to provide access to object properties. It's mostly a representation of .json files defining
 * object properties. It does not store instances of objects that appear in Rooms.
 */
class ObjectManager
{
	private:
		std::unordered_map<std::string, BackObject> objects;
		std::unordered_map<std::string, BackHoleObject> holeObjects;

	public:
		bool load();
		bool setupBgSprites(SpriteResource &mainSpriteRes, SpriteResource &lightSpriteRes, ResourceManager &resMgr,
							const struct back_obj_data &backObjData, enum LightObjectsState lightState) const;
		bool setupBgHoleSprites(SpriteResource &mainSpriteRes, SpriteResource &holeSpriteRes, bool &blend,
								ResourceManager &resMgr, const struct back_obj_data &backObjData) const;
};
