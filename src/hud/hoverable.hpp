// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022 h67ma <szycikm@gmail.com>

#pragma once

#include <SFML/System/Vector2.hpp>

class Hoverable
{
	public:
		virtual bool containsPoint(sf::Vector2i coords) = 0;
		virtual void setHover(bool hover) = 0;
};
