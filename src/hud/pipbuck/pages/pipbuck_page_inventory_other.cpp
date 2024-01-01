// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "pipbuck_page_inventory_other.hpp"

#include <string>

PipBuckPageInventoryOther::PipBuckPageInventoryOther(ResourceManager& resMgr) : PipBuckPage("Other") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.F, 500.F);
	this->dummy.setString("inv other");
}

void PipBuckPageInventoryOther::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->dummy, states);
}
