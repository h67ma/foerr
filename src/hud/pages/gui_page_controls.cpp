// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "gui_page_controls.hpp"

#include <string>

#include "../../settings/keymap.hpp"
#include "../../util/i18n.hpp"
#include "../log.hpp"

GuiPageControls::GuiPageControls(ResourceManager& resMgr) :
	GuiPage("Controls"), // TODO translate
	buttons({ { BTN_NORMAL, resMgr, POS_PAGE_BTN_BOTTOM_1, STR_SAVE, []() { Keymap::save(); } },
			  { BTN_NORMAL, resMgr, POS_PAGE_BTN_BOTTOM_2, STR_RESET_DEFAULT,
				[this]()
				{
					// TODO display confirm dialog
					Keymap::resetToDefault();
					this->updateDisplay();
					Log::i(STR_KEYMAP_RESETTED);
					Keymap::save();
				} } }),
	mappingDump(resMgr, FONT_FIXED, FONT_H3, { FULL_PAGE_WIDTH_SANS_SCROLLBAR, FULL_PAGE_HEIGHT_SANS_BOTTOM_BTNS },
				this->dumpList)
{
	for (auto& btn : this->buttons)
	{
		this->hoverMgr += &btn;
		this->clickMgr += &btn;
	}

	this->updateDisplay();
}

void GuiPageControls::updateDisplay()
{
	this->dumpList.clear();
	for (const auto item : Keymap::getKeyToActionMap())
	{
		this->dumpList.emplace_back(Keymap::actionToDisplayString(item.second) + " -> " +
										Keymap::keyToString(item.first),
									SettingsManager::hudColor);
	}

	this->mappingDump.handleItemsChanged();
}

bool GuiPageControls::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getPosition();

	if (this->hoverMgr.handleMouseMove(mousePos))
		return true;

	return this->mappingDump.handleMouseMove(mousePos);
}

ClickStatus GuiPageControls::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getPosition();

	ClickStatus status = this->clickMgr.handleLeftClick(clickPos);
	if (status != CLICK_NOT_CONSUMED)
		return status;

	return this->mappingDump.handleLeftClick(clickPos);
}

void GuiPageControls::handleLeftClickUp()
{
	this->mappingDump.handleLeftClickUp();
}

void GuiPageControls::handleScroll(float delta, sf::Vector2i mousePos)
{
	mousePos -= this->getPosition();

	this->mappingDump.handleScroll(delta, mousePos);
}

void GuiPageControls::handleSettingsChange()
{
	GuiPage::handleSettingsChange();

	for (auto& btn : this->buttons)
	{
		btn.handleSettingsChange();
	}

	this->mappingDump.handleSettingsChange();
}

void GuiPageControls::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	for (const auto& btn : this->buttons)
	{
		target.draw(btn, states);
	}

	target.draw(this->mappingDump, states);
}
