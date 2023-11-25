// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <SFML/System/Vector3.hpp>

#include "../util/util.hpp"

template<typename T>
struct HashableVector3 : public sf::Vector3<T>
{
	public:
		HashableVector3() : sf::Vector3<T>() {}
		HashableVector3(T x, T y, T z) : sf::Vector3<T>(x, y, z) {}

		bool operator==(const HashableVector3& other) const
		{
			// clang-format off
			return this->x == other.x &&
				   this->y == other.y &&
				   this->z == other.z;
			// clang-format on
		}
};

typedef HashableVector3<int> HashableVector3i;

template<typename T>
struct Vector3Hasher
{
		std::size_t operator()(HashableVector3<T> const& vec) const
		{
			std::size_t seed = 0;
			hash_combine(seed, vec.x);
			hash_combine(seed, vec.y);
			hash_combine(seed, vec.z);
			return seed;
		}
};
