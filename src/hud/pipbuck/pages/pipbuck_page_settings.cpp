#include "pipbuck_page_settings.hpp"

#include <string>

#include "../../../settings/settings_manager.hpp"
#include "../../../util/i18n.hpp"

PipBuckPageSettings::PipBuckPageSettings(ResourceManager &resMgr) :
	PipBuckPage("Settings"), // TODO translate
	buttons({
		{BTN_NORMAL, resMgr, { 400, 815 }, STR_SAVE, [this]() {
			// read new settings values from GUI controls on this page, save to SettingsManager

			SettingsManager::hudColor = this->hudColorSelector.getSelectedColor();
			SettingsManager::guiScale = this->guiScaleSlider.getValue();

			SettingsManager::saveConfig();
		}, CLICK_CONSUMED_SETTINGS_CHANGED},
		{BTN_NORMAL, resMgr, { 600, 815 }, STR_RESET_DEFAULT, [this]() {
			// reset all settings, write to file, update controls state on this page

			SettingsManager::resetAllToDefault();
			SettingsManager::saveConfig();

			this->hudColorSelector.setSelectedColor(SettingsManager::hudColor);
			this->guiScaleSlider.setValue(SettingsManager::guiScale);
		}, CLICK_CONSUMED_SETTINGS_CHANGED}
	}),
	hudColorSelector(*resMgr.getFont(FONT_NORMAL), SettingsManager::hudColor),
	guiScaleSlider(*resMgr.getFont(FONT_NORMAL), GUI_SCALE_MIN_VALUE, SettingsManager::guiScale, GUI_SCALE_MAX_VALUE, 2)
{
	for (auto &btn : this->buttons)
	{
		this->hoverMgr += &btn;
		this->clickMgr += &btn;
	}

	this->infoText.setFont(*resMgr.getFont(FONT_NORMAL));
	this->infoText.setCharacterSize(17);
	this->infoText.setPosition(400.F, 272.F);

	this->hudColorSelector.setPosition(500.F, 250.F);
	this->guiScaleSlider.setPosition(500.F, 345.F);

	// TODO would be cool if there was a button here "Open game directory" which opens file explorer in this dir
	this->infoText.setString(litSprintf("HUD color\n\n\nGUI scale\n\n\n\nMain game directory: %s\nSavegame directory: %s",
										SettingsManager::getGameRootDir().c_str(),
										SettingsManager::getSaveDir().c_str()));
}

ClickStatus PipBuckPageSettings::handleLeftClick(sf::Vector2i clickPos)
{
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
	if (this->hudColorSelector.handleMouseMove(mousePos))
		return true;

	if (this->guiScaleSlider.handleMouseMove(mousePos))
		return true;

	return this->hoverMgr.handleMouseMove(mousePos);
}

void PipBuckPageSettings::handleSettingsChange()
{
	for (auto &btn : this->buttons)
	{
		btn.handleSettingsChange();
	}

	this->hudColorSelector.handleSettingsChange();
	this->guiScaleSlider.handleSettingsChange();
	this->infoText.handleSettingsChange();
}

void PipBuckPageSettings::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (const auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}

	target.draw(this->infoText, states);
	target.draw(this->hudColorSelector, states);
	target.draw(this->guiScaleSlider, states);
}
