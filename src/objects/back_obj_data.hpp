// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <SFML/System/Vector2.hpp>

// representation of back object json node
struct back_obj_data
{
		std::string id;
		sf::Vector2u coordinates;
		int variantIdx;
};
