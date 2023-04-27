#pragma once

#include <cmath>

#include <SFML/System/Vector2.hpp>

#include "../consts.hpp"

enum ScreenCorner
{
	CORNER_TOP_LEFT,
	CORNER_TOP_RIGHT,
	CORNER_BOTTOM_LEFT,
	CORNER_BOTTOM_RIGHT,
	_CORNER_CNT
};

enum ScreenSide
{
	SIDE_TOP,
	SIDE_BOTTOM,
	_SIDE_CNT
};

constexpr uint FONT_SPAN = 16;
constexpr uint FONT_H3 = 18;
constexpr uint FONT_H2 = 23;
constexpr uint FONT_H1 = 40;

/**
 * Returns the vertical offset used for centering text.
 * The return value should be added to text Y coordinate.
 */
inline float getFontVOffset(float guiScale, uint fontSize)
{
	return std::round(guiScale * fontSize / -4);
}

sf::Vector2f calculateGuiAwarePoint(sf::Vector2f point);
