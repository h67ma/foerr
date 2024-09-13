// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "gui_page_settings.hpp"

#include <string>

#include "../../settings/settings_manager.hpp"
#include "../../util/i18n.hpp"

GuiPageSettings::GuiPageSettings(ResourceManager& resMgr) :
	GuiPage("Settings"), // TODO translate
	buttons({ { BTN_NORMAL, resMgr, POS_PAGE_BTN_BOTTOM_1, STR_SAVE,
				[this]()
				{
					// read new settings values from GUI controls on this page, save to SettingsManager

					SettingsManager::hudColor = this->hudColorSelector.getSelectedColor();
					SettingsManager::guiScale = this->guiScaleSlider.getValue();

					SettingsManager::saveConfig();
				},
				CLICK_CONSUMED_SETTINGS_CHANGED },
			  { BTN_NORMAL, resMgr, POS_PAGE_BTN_BOTTOM_2, STR_RESET_DEFAULT,
				[this]()
				{
					// reset all settings, write to file, update controls state on this page

					SettingsManager::resetAllToDefault();
					SettingsManager::saveConfig();
					this->updateControlsState();
				},
				CLICK_CONSUMED_SETTINGS_CHANGED } }),
	hudColorSelector(*resMgr.getFont(FONT_NORMAL), SettingsManager::hudColor),
	guiScaleSlider(SLIDER_HORIZONTAL, INPUT_SLIDER_LENGTH, *resMgr.getFont(FONT_NORMAL), true, GUI_SCALE_MIN_VALUE,
				   SettingsManager::guiScale, GUI_SCALE_MAX_VALUE, 2),
	infoText(*resMgr.getFont(FONT_NORMAL), 17U, { 0.F, 22.F })
{
	for (auto& btn : this->buttons)
	{
		this->hoverMgr += &btn;
		this->clickMgr += &btn;
	}

	this->hudColorSelector.setPosition(100.F, 0.F);
	this->guiScaleSlider.setPosition(100.F, 95.F);

	// TODO would be cool if there was a button here "Open game directory" which opens file explorer in this dir
	this->infoText.setString(
		litSprintf("HUD color\n\n\nGUI scale\n\n\n\nMain game directory: %s\nSavegame directory: %s",
				   SettingsManager::getGameRootDir().c_str(), SettingsManager::getSaveDir().c_str()));
}

/**
 * Update state of controls on this page to reflect current settings.
 */
void GuiPageSettings::updateControlsState()
{
	this->hudColorSelector.setSelectedColor(SettingsManager::hudColor);
	this->guiScaleSlider.setValue(SettingsManager::guiScale);
}

ClickStatus GuiPageSettings::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getPosition();

	if (this->hudColorSelector.handleLeftClick(clickPos))
		return CLICK_CONSUMED;

	if (this->guiScaleSlider.handleLeftClick(clickPos))
		return CLICK_CONSUMED;

	return this->clickMgr.handleLeftClick(clickPos);
}

void GuiPageSettings::handleLeftClickUp()
{
	this->hudColorSelector.handleLeftClickUp();
	this->guiScaleSlider.handleLeftClickUp();
}

bool GuiPageSettings::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getPosition();

	if (this->hudColorSelector.handleMouseMove(mousePos))
		return true;

	if (this->guiScaleSlider.handleMouseMove(mousePos))
		return true;

	return this->hoverMgr.handleMouseMove(mousePos);
}

void GuiPageSettings::handleSettingsChange()
{
	GuiPage::handleSettingsChange();

	for (auto& btn : this->buttons)
	{
		btn.handleSettingsChange();
	}

	this->hudColorSelector.handleSettingsChange();
	this->guiScaleSlider.handleSettingsChange();
	this->infoText.handleSettingsChange();

	// note: if this page initiated settings update, state of all controls should already be up to date.
	// this is for the case when settings are updated elsewhere, not on this page. notably main menu Settings page can
	// update settings and PipBuck Settings page must react to it, and vice versa.
	this->updateControlsState();
}

void GuiPageSettings::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	for (const auto& btn : this->buttons)
	{
		target.draw(btn, states);
	}

	target.draw(this->infoText, states);
	target.draw(this->hudColorSelector, states);
	target.draw(this->guiScaleSlider, states);
}
