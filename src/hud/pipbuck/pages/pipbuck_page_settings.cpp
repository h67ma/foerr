#include "pipbuck_page_settings.hpp"
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
}
