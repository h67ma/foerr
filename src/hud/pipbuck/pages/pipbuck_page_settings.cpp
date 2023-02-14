#include "pipbuck_page_settings.hpp"

#include <string>

#include "../../../settings/settings_manager.hpp"
#include "../../../util/i18n.hpp"

PipBuckPageSettings::PipBuckPageSettings(ResourceManager &resMgr) :
	PipBuckPage("Settings"), // TODO translate
	buttons({
		{BTN_NORMAL, resMgr, { 400, 815 }, STR_SAVE, []() {
			SettingsManager::saveConfig();
		}}
	})
{
	for (auto &btn : this->buttons)
	{
		this->hoverMgr += &btn;
		this->clickMgr += &btn;
	}

	this->infoText.setFont(*resMgr.getFont(FONT_NORMAL));
	this->infoText.setCharacterSize(17);
	this->infoText.setPosition(400.F, 250.F);

	// TODO would be cool if there was a button here "Open game directory" which opens file explorer in this dir
	this->infoText.setString(litSprintf("Main game directory: %s\nSavegame directory: %s",
										SettingsManager::getGameRootDir().c_str(),
										SettingsManager::getSaveDir().c_str()));
}

void PipBuckPageSettings::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}

	target.draw(this->infoText, states);
}
