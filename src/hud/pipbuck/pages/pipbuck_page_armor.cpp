#include "pipbuck_page_armor.hpp"
#include <string>

PipBuckPageArmor::PipBuckPageArmor(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("armor");
}

std::string PipBuckPageArmor::getLabel()
{
	return "Armor"; // TODO translate
}

void PipBuckPageArmor::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
