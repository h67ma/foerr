// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "pipbuck_page_ammo.hpp"

#include <string>

PipBuckPageAmmo::PipBuckPageAmmo(ResourceManager& resMgr) : PipBuckPage("Ammunition") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.F, 500.F);
	this->dummy.setString("ammo");
}

void PipBuckPageAmmo::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
