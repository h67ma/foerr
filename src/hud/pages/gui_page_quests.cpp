// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "gui_page_quests.hpp"

#include <string>

GuiPageQuests::GuiPageQuests(ResourceManager& resMgr) : GuiPage("Quests") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(100.F, 250.F);
	this->dummy.setString("quests");
}

void GuiPageQuests::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->dummy, states);
}
