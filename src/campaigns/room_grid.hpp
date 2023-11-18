// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <unordered_map>
#include <memory>

#include "../util/hashable_vector3.hpp"
#include "room.hpp"

/**
 * "The Grid. A digital frontier..."
 *
 * RoomGrid is used to store rooms for a single location, organized in a 3d grid (implemented as a map).
 *
 * The third dimension is needed to store backrooms. While it could be stored in a boolean (coords would be represented
 * by a struct of Vector2i + bool), storing all three in a single vector is convenient and also provides the possibility
 * of adding more Z layers. While having more than two Z layers is confusing, it may make sense for some locations.
 */
class RoomGrid
{
	private:
		HashableVector3i currentCoords = { 0, 0, 0 };
		std::unordered_map<HashableVector3<int>, std::shared_ptr<Room>, Vector3Hasher<int>> grid;

	public:
		HashableVector3i getCurrentCoords() const;
		void set(HashableVector3i coords, std::shared_ptr<Room> room);
		std::shared_ptr<Room> get(HashableVector3i coords) const;
		std::shared_ptr<Room> moveTo(HashableVector3i coords);
		std::shared_ptr<Room> moveToNear(Direction direction);
		void clear();
};
