// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

enum LightObjectsState
{
	LIGHTS_DEFAULT, // leave lights state up to randomness
	LIGHTS_ON, // force all lights to be turned on
	LIGHTS_OFF, // force all lights to be turned off
};

constexpr enum LightObjectsState lonToLightObjectsState(int lon)
{
	if (lon > 0)
		return LIGHTS_ON;

	if (lon < 0)
		return LIGHTS_OFF;

	return LIGHTS_DEFAULT;
}
