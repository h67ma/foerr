#include "pipbuck_page_weapons.hpp"
#include <string>

PipBuckPageWeapons::PipBuckPageWeapons(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("weapons");
}

std::string PipBuckPageWeapons::getLabel() const
{
	return "Weapons"; // TODO translate
}

void PipBuckPageWeapons::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
