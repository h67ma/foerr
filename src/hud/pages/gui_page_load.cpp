// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "gui_page_load.hpp"

#include <string>

#include "../../util/i18n.hpp"

/**
 * @param resMgr reference to resource manager
 * @param isInMainMenu true if this page is displayed in main menu, false if in PipBuck
 */
GuiPageLoad::GuiPageLoad(ResourceManager& resMgr, bool isInMainMenu) :
	GuiPage("Load"), // TODO translate
	isInMainMenu(isInMainMenu),
	exitBtn(
		BTN_NORMAL, resMgr, { 400, 815 }, STR_EXIT_TO_MAIN_MENU,
		[]()
		{
			// TODO display confirm box
			// TODO also save game before unloading campaign
		},
		CLICK_CONSUMED_UNLOAD)
{
	if (!this->isInMainMenu)
	{
		this->hoverMgr += &this->exitBtn;
		this->clickMgr += &this->exitBtn;
	}
}

bool GuiPageLoad::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getPosition();

	return this->hoverMgr.handleMouseMove(mousePos);
}

ClickStatus GuiPageLoad::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getPosition();

	return this->clickMgr.handleLeftClick(clickPos);
}

void GuiPageLoad::handleSettingsChange()
{
	GuiPage::handleSettingsChange();

	if (!this->isInMainMenu)
		this->exitBtn.handleSettingsChange();
}

void GuiPageLoad::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	if (!this->isInMainMenu)
		target.draw(this->exitBtn, states);
}
