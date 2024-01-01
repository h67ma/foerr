// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "pipbuck_page_save.hpp"

#include <string>

PipBuckPageSave::PipBuckPageSave(ResourceManager& resMgr) : PipBuckPage("Save") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.F, 500.F);
	this->dummy.setString("save");
}

void PipBuckPageSave::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->dummy, states);
}
