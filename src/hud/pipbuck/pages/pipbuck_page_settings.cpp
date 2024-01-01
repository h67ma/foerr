// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "pipbuck_page_settings.hpp"

#include <string>

#include "../../../settings/settings_manager.hpp"
#include "../../../util/i18n.hpp"

PipBuckPageSettings::PipBuckPageSettings(ResourceManager& resMgr) :
	PipBuckPage("Settings"), // TODO translate
	buttons({ { BTN_NORMAL,
				resMgr,
				{ 400, 815 },
				STR_SAVE,
				[this]()
				{
					// read new settings values from GUI controls on this page, save to SettingsManager

					SettingsManager::hudColor = this->hudColorSelector.getSelectedColor();
					SettingsManager::guiScale = this->guiScaleSlider.getValue();

					SettingsManager::saveConfig();
				},
				CLICK_CONSUMED_SETTINGS_CHANGED },
			  { BTN_NORMAL,
				resMgr,
				{ 600, 815 },
				STR_RESET_DEFAULT,
				[this]()
				{
					// reset all settings, write to file, update controls state on this page

					SettingsManager::resetAllToDefault();
					SettingsManager::saveConfig();
					this->updateControlsState();
				},
				CLICK_CONSUMED_SETTINGS_CHANGED } }),
	hudColorSelector(*resMgr.getFont(FONT_NORMAL), SettingsManager::hudColor),
	guiScaleSlider(*resMgr.getFont(FONT_NORMAL), GUI_SCALE_MIN_VALUE, SettingsManager::guiScale, GUI_SCALE_MAX_VALUE,
				   2),
	infoText(*resMgr.getFont(FONT_NORMAL), 17U, { 400.F, 272.F })
{
	for (auto& btn : this->buttons)
	{
		this->hoverMgr += &btn;
		this->clickMgr += &btn;
	}

	this->hudColorSelector.setPosition(500.F, 250.F);
	this->guiScaleSlider.setPosition(500.F, 345.F);

	// TODO would be cool if there was a button here "Open game directory" which opens file explorer in this dir
	this->infoText.setString(
		litSprintf("HUD color\n\n\nGUI scale\n\n\n\nMain game directory: %s\nSavegame directory: %s",
				   SettingsManager::getGameRootDir().c_str(), SettingsManager::getSaveDir().c_str()));
}

/**
 * Update state of controls on this page to reflect current settings.
 */
void PipBuckPageSettings::updateControlsState()
{
	this->hudColorSelector.setSelectedColor(SettingsManager::hudColor);
	this->guiScaleSlider.setValue(SettingsManager::guiScale);
}

ClickStatus PipBuckPageSettings::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getPosition();

	if (this->hudColorSelector.handleLeftClick(clickPos))
		return CLICK_CONSUMED;

	if (this->guiScaleSlider.handleLeftClick(clickPos))
		return CLICK_CONSUMED;

	return this->clickMgr.handleLeftClick(clickPos);
}

void PipBuckPageSettings::handleLeftClickUp()
{
	this->hudColorSelector.handleLeftClickUp();
	this->guiScaleSlider.handleLeftClickUp();
}

bool PipBuckPageSettings::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getPosition();

	if (this->hudColorSelector.handleMouseMove(mousePos))
		return true;

	if (this->guiScaleSlider.handleMouseMove(mousePos))
		return true;

	return this->hoverMgr.handleMouseMove(mousePos);
}

void PipBuckPageSettings::handleSettingsChange()
{
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

void PipBuckPageSettings::draw(sf::RenderTarget& target, sf::RenderStates states) const
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
