// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <SFML/System/Vector2.hpp>

/**
 * Normally only CLICK_NOT_CONSUMED and CLICK_CONSUMED should be used. The remaining values are used to pass a message
 * back through the caller hierarchy to inform that a certain action should be performed. For example a button on some
 * PipBuck page is used to close the PipBuck. To do this, it would require a reference to PipBuck object, but actually
 * PipBuck contains components which in turn contain the button, so it would be a bit awkward. Instead, the button
 * returns a special status on click (in this example CLICK_CONSUMED_CLOSE). It will propagate to a caller that checks
 * for this value and it can then take appropriate action (in this case PipBuck will just close itself).
 */
enum ClickStatus
{
	CLICK_NOT_CONSUMED,
	CLICK_CONSUMED, // omnomnom
	CLICK_CONSUMED_CLOSE, // PipBuck should be closed
	CLICK_CONSUMED_UNLOAD, // PipBuck should unload its campaign infos
	CLICK_CONSUMED_SETTINGS_CHANGED, // components should update their state after settings change
};

class Clickable
{
	public:
		virtual bool containsPoint(sf::Vector2i coords) = 0;
		virtual ClickStatus handleLeftClick(sf::Vector2i clickPos) = 0;
};
