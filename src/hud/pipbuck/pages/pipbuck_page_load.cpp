// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "pipbuck_page_load.hpp"

#include <string>

#include "../../../util/i18n.hpp"

/**
 * @param resMgr reference to resource manager
 * @param isInMainMenu true if this page is displayed in main menu, false if in PipBuck
 */
PipBuckPageLoad::PipBuckPageLoad(ResourceManager& resMgr, bool isInMainMenu) :
	PipBuckPage("Load"), // TODO translate
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

void PipBuckPageLoad::handleSettingsChange()
{
	PipBuckPage::handleSettingsChange();

	if (!this->isInMainMenu)
		this->exitBtn.handleSettingsChange();
}

void PipBuckPageLoad::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	if (!this->isInMainMenu)
		target.draw(this->exitBtn, states);
}
