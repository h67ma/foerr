#include "pipbuck_page_ammo.hpp"
#include <string>

PipBuckPageAmmo::PipBuckPageAmmo(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("ammo");
}

std::string PipBuckPageAmmo::getLabel() const
{
	return "Ammunition"; // TODO translate
}

void PipBuckPageAmmo::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
