// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "gui_transformable.hpp"

#include "hud.hpp"

void GuiTransformable::setPosition(float x, float y)
{
	this->origPos.x = x;
	this->origPos.y = y;

	this->handleGuiScaleChange();
}

void GuiTransformable::setPosition(const sf::Vector2f& position)
{
	this->origPos = position;

	this->handleGuiScaleChange();
}

/**
 * Updates the position using saved "original position" and current GUI scale.
 *
 * Should be called by components inheriting this in ::handleSettingsChange().
 */
void GuiTransformable::handleGuiScaleChange()
{
	sf::Transformable::setPosition(calculateGuiAwarePoint(this->origPos));
}
