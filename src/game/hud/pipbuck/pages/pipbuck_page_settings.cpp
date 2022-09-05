#include "pipbuck_page_settings.hpp"

PipBuckPageSettings::PipBuckPageSettings(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("settings");
}

std::string PipBuckPageSettings::getLabel()
{
	return "Settings"; // TODO translate
}

void PipBuckPageSettings::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
