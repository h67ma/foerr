// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <unordered_map>
#include <string>

#include <nlohmann/json.hpp>

#include "material.hpp"

// TODO find the exact value, or just store it in materials.json
#define LIQUID_OPACITY 0x9A

/**
 * Material Manager stores information about materials and allows retrieving it. This data must be loaded from a file.
 *
 * The data is split into two maps because of how Rooms are encoded. A single material equals a single ASCII character,
 * which (considering only writeable characters) leaves only 95 usable symbols. This range can run out if we used all
 * in a single map, so we use one map for solids, and second map for everything else. Technically we could store
 * all materials in a single map in the program, but since they are already nicely separated why not keep them that way.
 */
class MaterialManager
{
	private:
		std::unordered_map<char, struct material> mapSolids;
		std::unordered_map<char, struct material> mapOthers;
		bool loadMap(const nlohmann::json &root, std::unordered_map<char, struct material> &theMap,
					 const char* nodeKey);

	public:
		bool load();
		const struct material* getSolid(char materialName) const;
		const struct material* getOther(char materialName) const;
};
