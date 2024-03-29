// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "button.hpp"

#include <utility>

#include "../../util/util.hpp"

Button::Button(sf::Vector2u position, std::function<void(void)> callback, ClickStatus consumedStatus) :
	callback(std::move(callback)),
	consumedStatus(consumedStatus)
{
	// we use uints because we don't want to have buttons
	// "in between", because it may cause edges to blur
	this->setPosition(static_cast<sf::Vector2f>(position));
}

/**
 * Checks if click was placed inside button area.
 * If it was, and the callback function exists, the callback will be called.
 */
ClickStatus Button::handleLeftClick(sf::Vector2i clickPos)
{
	if (!this->containsPoint(clickPos))
		return CLICK_NOT_CONSUMED;

	if (this->callback != nullptr)
		this->callback();

	return this->consumedStatus;
}
