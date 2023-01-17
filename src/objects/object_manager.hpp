#pragma once

#include <unordered_map>
#include <string>

#include "back_hole_obj.hpp"
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
							const std::string &objId, int variantIdx) const;
		bool setupBgHoleSprites(SpriteResource &mainSpriteRes, SpriteResource &holeSpriteRes, bool &blend,
								ResourceManager &resMgr, const std::string &objId, int variantIdx) const;
};
