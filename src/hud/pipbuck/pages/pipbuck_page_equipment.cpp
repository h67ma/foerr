#include "pipbuck_page_equipment.hpp"
#include <string>

PipBuckPageEquipment::PipBuckPageEquipment(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("equipment");
}

std::string PipBuckPageEquipment::getLabel() const
{
	return "Equipment"; // TODO translate
}

void PipBuckPageEquipment::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
