// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "gui_page_effects.hpp"

#include <string>

GuiPageEffects::GuiPageEffects(ResourceManager& resMgr) : GuiPage("Effects") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(100.F, 250.F);
	this->dummy.setString("effects");
}

void GuiPageEffects::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->dummy, states);
}
