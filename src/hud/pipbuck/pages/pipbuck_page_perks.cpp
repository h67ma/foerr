// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "pipbuck_page_perks.hpp"

#include <string>

PipBuckPagePerks::PipBuckPagePerks(ResourceManager &resMgr) :
	PipBuckPage("Perks") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.F, 500.F);
	this->dummy.setString("perks");
}

void PipBuckPagePerks::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
