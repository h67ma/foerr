#include "pipbuck_page_inventory_other.hpp"
#include <string>

PipBuckPageInventoryOther::PipBuckPageInventoryOther(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("inv other");
}

std::string PipBuckPageInventoryOther::getLabel()
{
	return "Other"; // TODO translate
}

void PipBuckPageInventoryOther::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
