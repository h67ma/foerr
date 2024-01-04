// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "gui_page.hpp"

GuiPage::GuiPage(const std::string& pageTitle) : pageTitle(pageTitle) {}

ClickStatus GuiPage::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getPosition();

	return this->clickMgr.handleLeftClick(clickPos);
}

bool GuiPage::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getPosition();

	return this->hoverMgr.handleMouseMove(mousePos);
}

void GuiPage::handleSettingsChange()
{
	this->handleGuiScaleChange();
}
