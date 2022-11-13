#include "pipbuck_page_settings.hpp"
#include <string>
#include "../../../util/i18n.hpp"

PipBuckPageSettings::PipBuckPageSettings(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, SettingsManager &settings) :
	buttons({
		{scale, BTN_NORMAL, hudColor, resMgr, { 400, 815 }, STR_SAVE, [&settings]() {
			settings.saveConfig();
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
	this->infoText.setPosition(400.f, 250.f);

	// TODO would be cool if there was a button here "Open game directory" which opens file explorer in this dir
	this->infoText.setString(litSprintf("Main game directory: %s\nSavegame directory: %s",
										settings.getGameRootDir().c_str(),
										settings.getSaveDir().c_str()));
}

std::string PipBuckPageSettings::getLabel()
{
	return "Settings"; // TODO translate
}

void PipBuckPageSettings::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}

	target.draw(this->infoText, states);
}
